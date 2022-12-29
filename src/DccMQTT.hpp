/*
 * © 2021 Gregor Baues. All rights reserved.
 *
 * This is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * See the GNU General Public License for more details
 * <https://www.gnu.org/licenses/>
 */

#ifndef DCCMQTT_H
#define DCCMQTT_H

#include <mqtt/async_client.h>
#include <mqtt/topic.h>
#include "ShellCmdExec.hpp"
#include "Diag.hpp"

const int QOS = 0;
const int N_RETRY_ATTEMPTS = 5;

#define TOPIC "dcccli"
#define CLIENT_ID "dcccli"

//----------------------
// MQTT Broker
//----------------------

struct MQTTBroker
{
	std::string brokerUri; // domain:port or IP:port
	std::string user;
	std::string pwd;
	std::string prefix;

	MQTTBroker(std::string uri) : brokerUri(uri){};
};

// standard public mosquitto org broker for testing purposes not for production purposes
// as all topic can be read/published to by anyone no use / no pwd

const MQTTBroker publicBroker("tcp://test.mosquitto.org:1883");

class action_listener : public virtual mqtt::iaction_listener
{
	std::string name_;

	void on_failure(const mqtt::token &tok) override
	{
		std::cout << name_ << " failure";
		if (tok.get_message_id() != 0)
			std::cout << " for token: [" << tok.get_message_id() << "]" << std::endl;
		std::cout << std::endl;
	}

	void on_success(const mqtt::token &tok) override
	{
		std::cout << name_ << " success";
		if (tok.get_message_id() != 0)
			std::cout << " for token: [" << tok.get_message_id() << "]" << std::endl;
		auto top = tok.get_topics();
		if (top && !top->empty())
			std::cout << "\ttoken topic: '" << (*top)[0] << "', ..." << std::endl;
		std::cout << std::endl;
	}

public:
	action_listener(const std::string &name) : name_(name) {}
};

/**
 * Local callback & listener class for use with the client connection.
 * This is primarily intended to receive messages, but it will also monitor
 * the connection to the broker. If the connection is lost, it will attempt
 * to restore the connection and re-subscribe to the topic.
 */
class callback : public virtual mqtt::callback,
				 public virtual mqtt::iaction_listener

{
	// Counter for the number of connection retries
	int nretry_;
	// The MQTT client
	mqtt::async_client &cli_;
	// Options to use if we need to reconnect
	mqtt::connect_options &connOpts_;
	// An action listener to display the result of actions.
	action_listener subListener_;

	// This deomonstrates manually reconnecting to the broker by calling
	// connect() again. This is a possibility for an application that keeps
	// a copy of it's original connect_options, or if the app wants to
	// reconnect with different options.
	// Another way this can be done manually, if using the same options, is
	// to just call the async_client::reconnect() method.
	void reconnect()
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(2500));
		try
		{
			cli_.connect(connOpts_, nullptr, *this);
		}
		catch (const mqtt::exception &exc)
		{
			std::cerr << "Error: " << exc.what() << std::endl;
			exit(1);
		}
	}

	// Re-connection failure
	void on_failure(const mqtt::token &tok) override
	{
		std::cout << "Connection attempt failed" << std::endl;
		if (++nretry_ > N_RETRY_ATTEMPTS)
			exit(1);
		reconnect();
	}

	// (Re)connection success
	// Either this or connected() can be used for callbacks.
	void on_success(const mqtt::token &tok) override {}

	// (Re)connection success
	void connected(const std::string &cause) override
	{

		INFO("Connected.");
		// auto topicOpts = mqtt::topic_options_builder()
		// cli_.subscribe(TOPIC, DFLT_QOS, nullptr, subListener_);
	}

	// Callback for when the connection is lost.
	// This will initiate the attempt to manually reconnect.
	void connection_lost(const std::string &cause) override
	{
		std::cout << "\nConnection lost" << std::endl;
		if (!cause.empty())
			std::cout << "\tcause: " << cause << std::endl;

		std::cout << "Reconnecting..." << std::endl;
		nretry_ = 0;
		reconnect();
	}

	// Callback for when a message arrives.
	void message_arrived(mqtt::const_message_ptr msg) override
	{
		std::cout << "Message arrived" << std::endl;
		std::cout << "\ttopic: '" << msg->get_topic() << "'" << std::endl;
		std::cout << "\tpayload: '" << msg->to_string() << "'\n"
				  << std::endl;
	}

	void delivery_complete(mqtt::delivery_token_ptr token) override {}

public:
	callback(mqtt::async_client &cli, mqtt::connect_options &connOpts)
		: nretry_(0), cli_(cli), connOpts_(connOpts), subListener_("Subscription") {}
};

//---------

class DccMQTT
{
private:
	std::string clientID = "dcccli";
	MQTTBroker broker = publicBroker; // by default the public broker
	std::string topic = "foobar";

public:
	DccMQTT() = default;
	~DccMQTT() = default;

	bool connect()
	{
		mqtt::async_client cli(broker.brokerUri, clientID, nullptr);

		auto connOpts = mqtt::connect_options_builder()
							.clean_session(false)
							.finalize();

		callback cb(cli, connOpts);
		cli.set_callback(cb);

		try
		{
			INFO("Connecting to MQTT broker at: {}", broker.brokerUri);
			// cli.connect(connOpts, nullptr, cb);
			mqtt::token_ptr conntok = cli.connect(connOpts, nullptr, cb);
			conntok->wait();

			INFO("Subscribing to topic: [{}] for client [{}] using QoS [{}]", TOPIC, CLIENT_ID, QOS);

			mqtt::token_ptr tok;
			mqtt::topic top(cli, TOPIC, QOS);

			std::string payload = "foobarcli";
			tok = top.publish(payload);
			tok->wait();

			auto subOpts = mqtt::subscribe_options(true);
			top.subscribe(subOpts)->wait();
			std::cout << "Ok" << std::endl;
		}
		catch (const mqtt::exception &exc)
		{
			auto s = fmt::format("ERROR: Unable to connect to MQTT server: {}/{}\n", broker.brokerUri, exc.what());
			throw ShellCmdExecException(s);
		}

		return DCC_SUCCESS;
	}
};

#endif