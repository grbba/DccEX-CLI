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

#pragma once

#ifdef WIN32
    #include "DccModelWin32.hpp"
#else
    #include "DccModelUnix.hpp"
#endif

// #include <stdexcept>
// #include <regex>

// #include "nlohmann/json.hpp"


// namespace nlohmann {
//     template <typename T>
//     struct adl_serializer<std::shared_ptr<T>> {
//         static void to_json(json& j, const std::shared_ptr<T>& opt) {
//             if (!opt) j = nullptr; else j = *opt;
//         }

     
//         static std::shared_ptr<T> from_json(const json& j) {
//             if (j.is_null()) return std::unique_ptr<T>(); else return std::unique_ptr<T>(new T(j.get<T>()));
//         }
//     };
// }

// namespace DccModel {
//     using nlohmann::json;

//     inline json get_untyped(const json &j, const char *property) {
//         if (j.find(property) != j.end()) {
//             return j.at(property).get<json>();
//         }
//         return json();
//     }

//     template <typename T>
//     inline std::shared_ptr<T> get_optional(const json &j, const char *property) {
//         if (j.find(property) != j.end()) {
//             return j.at(property).get<std::shared_ptr<T>>();
//         }
//         return std::shared_ptr<T>();
//     }

//     /** 
//      * Root Object for all instances of the Layout model 
//      * 
//      */
//     class DccObject
//     {

//     public:
//         DccObject() = default;
//         virtual ~DccObject() = default;

//     private:
//         int64_t _dccid;

//     public:
//         int64_t get_dccid() const { return _dccid; }
//         void set_dccid(int64_t value) { this->_dccid = value; }
//     };

//     /**
//      * From connection
//      */
//     class From  : public DccObject {
//         public:
//         From() = default;
//         virtual ~From() = default;

//         private:
//         std::string from_module;
//         int64_t path;

//         public:
//         /**
//          * Module name of the path from which to connect
//          */
//         const std::string & get_from_module() const { return from_module; }
//         std::string & get_mutable_from_module() { return from_module; }
//         void set_from_module(const std::string& value) { this->from_module = value; }

//         /**
//          * Path id from which to connect
//          */
//         const int64_t & get_path() const { return path; }
//         int64_t & get_mutable_path() { return path; }
//         void set_path(const int64_t& value) { this->path = value; }
//     };

//     /**
//      * To connection
//      */
//     class To  : public DccObject {
//         public:
//         To() = default;
//         virtual ~To() = default;

//         private:
//         std::string to_module;
//         int64_t path;

//         public:
//         /**
//          * Module name of the path to which to connect
//          */
//         const std::string & get_to_module() const { return to_module; }
//         std::string & get_mutable_to_module() { return to_module; }
//         void set_to_module(const std::string& value) { this->to_module = value; }

//         /**
//          * Path id to which to connect
//          */
//         const int64_t & get_path() const { return path; }
//         int64_t & get_mutable_path() { return path; }
//         void set_path(const int64_t& value) { this->path = value; }
//     };

//     /**
//      * An entry in the junction list between to modules
//      */
//     class Junction  : public DccObject {
//         public:
//         Junction() = default;
//         virtual ~Junction() = default;

//         private:
//         From from;
//         To to;

//         public:
//         /**
//          * From connection
//          */
//         const From & get_from() const { return from; }
//         From & get_mutable_from() { return from; }
//         void set_from(const From& value) { this->from = value; }

//         /**
//          * To connection
//          */
//         const To & get_to() const { return to; }
//         To & get_mutable_to() { return to; }
//         void set_to(const To& value) { this->to = value; }
//     };

//     /**
//      * Description of the Layout incl. name of the layout a short text describing the layout and
//      * the version. There can be only one layout per definition file
//      */
//     class Layout : public DccObject {
//         public:
//         Layout() = default;
//         virtual ~Layout() = default;

//         private:
//         std::shared_ptr<std::string> author;
//         std::shared_ptr<std::string> description;
//         std::string name;
//         std::string version;

//         public:
//         /**
//          * Author of this layout (optional)
//          */
//         std::shared_ptr<std::string> get_author() const { return author; }
//         void set_author(std::shared_ptr<std::string> value) { this->author = value; }

//         /**
//          * A short description of the layout (optional)
//          */
//         std::shared_ptr<std::string> get_description() const { return description; }
//         void set_description(std::shared_ptr<std::string> value) { this->description = value; }

//         /**
//          * Name of this layout
//          */
//         const std::string & get_name() const { return name; }
//         std::string & get_mutable_name() { return name; }
//         void set_name(const std::string& value) { this->name = value; }

//         /**
//          * Version of this layout definition in the form of x.y.z | x,y,z being numbers
//          */
//         const std::string & get_version() const { return version; }
//         std::string & get_mutable_version() { return version; }
//         void set_version(const std::string& value) { this->version = value; }
//     };

//     /**
//      * A Module with a name, an id and a trackplan
//      */
//     class Module  : public DccObject {
//         public:
//         Module() = default;
//         virtual ~Module() = default;

//         private:
//         std::shared_ptr<std::string> description;
//         std::string name;
//         std::string trackplan;

//         public:
//         /**
//          * Short description of the module (optional)
//          */
//         std::shared_ptr<std::string> get_description() const { return description; }
//         void set_description(std::shared_ptr<std::string> value) { this->description = value; }

//         /**
//          * Name of the module
//          */
//         const std::string & get_name() const { return name; }
//         std::string & get_mutable_name() { return name; }
//         void set_name(const std::string& value) { this->name = value; }

//         /**
//          * Every module has a trackplan identified by its name.
//          */
//         const std::string & get_trackplan() const { return trackplan; }
//         std::string & get_mutable_trackplan() { return trackplan; }
//         void set_trackplan(const std::string& value) { this->trackplan = value; }
//     };

//     /**
//      * End of a track closed by a bumper
//      */
//     class Bumper  : public DccObject {
//         public:
//         Bumper() = default;
//         virtual ~Bumper() = default;

//         private:
//         std::shared_ptr<std::string> description;
//         int64_t path;

//         public:
//         /**
//          * Short text about the bumper e.g. 'End of refueling siding'.
//          */
//         std::shared_ptr<std::string> get_description() const { return description; }
//         void set_description(std::shared_ptr<std::string> value) { this->description = value; }

//         /**
//          * The id of the bumper. Not a path per se as there is only one node id. The Id is used to
//          * link to any other track element such as rail or Turnout
//          */
//         const int64_t & get_path() const { return path; }
//         int64_t & get_mutable_path() { return path; }
//         void set_path(const int64_t& value) { this->path = value; }
//     };

//     /**
//      * Defines a crossing by means of four nodes; Length shall always be 0
//      */
//     class Crossing  : public DccObject {
//         public:
//         Crossing() = default;
//         virtual ~Crossing() = default;

//         private:
//         std::shared_ptr<std::string> description;
//         int64_t length;
//         std::vector<int64_t> path;
//         std::vector<int64_t> slip;

//         public:
//         /**
//          * Simple crossing, single or double slip
//          */
//         std::shared_ptr<std::string> get_description() const { return description; }
//         void set_description(std::shared_ptr<std::string> value) { this->description = value; }

//         /**
//          * Length of the crossing. Should always be 0 as we can't reasonably store trains on the
//          * crossings without blocking other routes/traffic
//          */
//         const int64_t & get_length() const { return length; }
//         int64_t & get_mutable_length() { return length; }
//         void set_length(const int64_t& value) { this->length = value; }

//         /**
//          * Nodes of the crossing specified in clock wise order starting top left.
//          */
//         const std::vector<int64_t> & get_path() const { return path; }
//         std::vector<int64_t> & get_mutable_path() { return path; }
//         void set_path(const std::vector<int64_t>& value) { this->path = value; }

//         /**
//          * Possible slip start or end points. The second slip point will be calculated automatically
//          * from the crossing path nodes. If empty we have a simple crossing
//          */
//         const std::vector<int64_t> & get_slip() const { return slip; }
//         std::vector<int64_t> & get_mutable_slip() { return slip; }
//         void set_slip(const std::vector<int64_t>& value) { this->slip = value; }
//     };

//     /**
//      * A piece of single track having two end points and nothing in between them.
//      */
//     class Rail  : public DccObject {
//         public:
//         Rail() = default;
//         virtual ~Rail() = default;

//         private:
//         std::shared_ptr<std::string> description;
//         int64_t length;
//         std::vector<int64_t> path;

//         public:
//         /**
//          * Short text about the track segment.
//          */
//         std::shared_ptr<std::string> get_description() const { return description; }
//         void set_description(std::shared_ptr<std::string> value) { this->description = value; }

//         /**
//          * Length of the track segment
//          */
//         const int64_t & get_length() const { return length; }
//         int64_t & get_mutable_length() { return length; }
//         void set_length(const int64_t& value) { this->length = value; }

//         /**
//          * Array of two node id's representing a segment of track
//          */
//         const std::vector<int64_t> & get_path() const { return path; }
//         std::vector<int64_t> & get_mutable_path() { return path; }
//         void set_path(const std::vector<int64_t>& value) { this->path = value; }
//     };

//     /**
//      * Defines a turnout by means of three nodes; 2 on the wide end and 1 on the narrow end.
//      * Length shall always be 0
//      */
//     class Turnout  : public DccObject {
//         public:
//         Turnout() = default;
//         virtual ~Turnout() = default;

//         private:
//         std::shared_ptr<std::string> description;
//         int64_t length;
//         int64_t narrow;
//         std::vector<int64_t> wide;

//         public:
//         /**
//          * The narrow end of the turnout i.e. the part opposite to the two other end points ( wide
//          * end )
//          */
//         std::shared_ptr<std::string> get_description() const { return description; }
//         void set_description(std::shared_ptr<std::string> value) { this->description = value; }

//         /**
//          * Length of the turnout. Should always be 0 as we can't reasonably store trains on the
//          * turnouts without blocking other routes/traffic
//          */
//         const int64_t & get_length() const { return length; }
//         int64_t & get_mutable_length() { return length; }
//         void set_length(const int64_t& value) { this->length = value; }

//         /**
//          * The narrow end of the turnout i.e. the part opposite to the two other end points ( wide
//          * end ) represented by a node id (number)
//          */
//         const int64_t & get_narrow() const { return narrow; }
//         int64_t & get_mutable_narrow() { return narrow; }
//         void set_narrow(const int64_t& value) { this->narrow = value; }

//         /**
//          * The wide end of the turnout. i.e. the two end points opposite to the single other end
//          * points (narrow end)
//          */
//         const std::vector<int64_t> & get_wide() const { return wide; }
//         std::vector<int64_t> & get_mutable_wide() { return wide; }
//         void set_wide(const std::vector<int64_t>& value) { this->wide = value; }
//     };

//     /**
//      * Bumper
//      *
//      * Track segment
//      *
//      * A turnout with two or three connections
//      *
//      * A crossing with four connections
//      */
//     class Trackelement  : public DccObject {
//         public:
//         Trackelement() = default;
//         virtual ~Trackelement() = default;

//         private:
//         std::shared_ptr<Bumper> bumper;
//         std::shared_ptr<Rail> rail;
//         std::shared_ptr<Turnout> turnout;
//         std::shared_ptr<Crossing> crossing;

//         public:
//         /**
//          * End of a track closed by a bumper
//          */
//         std::shared_ptr<Bumper> get_bumper() const { return bumper; }
//         void set_bumper(std::shared_ptr<Bumper> value) { this->bumper = value; }

//         /**
//          * A piece of single track having two end points and nothing in between them.
//          */
//         std::shared_ptr<Rail> get_rail() const { return rail; }
//         void set_rail(std::shared_ptr<Rail> value) { this->rail = value; }

//         /**
//          * Defines a turnout by means of three nodes; 2 on the wide end and 1 on the narrow end.
//          * Length shall always be 0
//          */
//         std::shared_ptr<Turnout> get_turnout() const { return turnout; }
//         void set_turnout(std::shared_ptr<Turnout> value) { this->turnout = value; }

//         /**
//          * Defines a crossing by means of four nodes; Length shall always be 0
//          */
//         std::shared_ptr<Crossing> get_crossing() const { return crossing; }
//         void set_crossing(std::shared_ptr<Crossing> value) { this->crossing = value; }
//     };

//     /**
//      * A track section is a collection of track elements ( bumper, turnout, rail) which are
//      * connected to each other
//      */
//     class Tracksection   : public DccObject{
//         public:
//         Tracksection() = default;
//         virtual ~Tracksection() = default;

//         private:
//         std::string name;
//         std::vector<Trackelement> trackelements;

//         public:
//         /**
//          * Name of the track section e.g. main or industry etc...
//          */
//         const std::string & get_name() const { return name; }
//         std::string & get_mutable_name() { return name; }
//         void set_name(const std::string& value) { this->name = value; }

//         /**
//          * List of all the track elements of the track section (bumper, rail or turnout).
//          */
//         const std::vector<Trackelement> & get_trackelements() const { return trackelements; }
//         std::vector<Trackelement> & get_mutable_trackelements() { return trackelements; }
//         void set_trackelements(const std::vector<Trackelement>& value) { this->trackelements = value; }
//     };

//     /**
//      * A track plan is described by its name which is used in a module description and it's
//      * track sections.
//      */
//     class Trackplan  : public DccObject {
//         public:
//         Trackplan() = default;
//         virtual ~Trackplan() = default;

//         private:
//         std::shared_ptr<std::string> description;
//         std::string name;
//         std::vector<Tracksection> tracksections;

//         public:
//         /**
//          * Short text describing the track plan (optional)
//          */
//         std::shared_ptr<std::string> get_description() const { return description; }
//         void set_description(std::shared_ptr<std::string> value) { this->description = value; }

//         /**
//          * Name of the track plan to be used at the module level
//          */
//         const std::string & get_name() const { return name; }
//         std::string & get_mutable_name() { return name; }
//         void set_name(const std::string& value) { this->name = value; }

//         /**
//          * List of track sections for this track plan. Track sections are user defined and represent
//          * some logical design of the layout. iin its simplest form the full track plan has only one
//          * track section
//          */
//         const std::vector<Tracksection> & get_tracksections() const { return tracksections; }
//         std::vector<Tracksection> & get_mutable_tracksections() { return tracksections; }
//         void set_tracksections(const std::vector<Tracksection>& value) { this->tracksections = value; }
//     };

//     /**
//      * The root DccLayout Schema comprises the entire JSON document i.e. provides an entry point
//      * to acces to all layout elements
//      */
//     class DccExLayout {
//         public:
//         DccExLayout() = default;
//         virtual ~DccExLayout() = default;

//         private:
//         std::shared_ptr<std::vector<Junction>> junctions;
//         Layout layout;
//         std::vector<Module> modules;
//         std::vector<Trackplan> trackplans;
//         nlohmann::json schema;

//         public:
//         /**
//          * An array of junctions between modules
//          */
//         std::shared_ptr<std::vector<Junction>> get_junctions() const { return junctions; }
//         void set_junctions(std::shared_ptr<std::vector<Junction>> value) { this->junctions = value; }

//         /**
//          * Description of the Layout incl. name of the layout a short text describing the layout and
//          * the version. There can be only one layout per definition file
//          */
//         const Layout & get_layout() const { return layout; }
//         Layout & get_mutable_layout() { return layout; }
//         void set_layout(const Layout& value) { this->layout = value; }

//         /**
//          * A Layout is composed by at least one or more physical modules. Modules are defined in an
//          * array and have to have a name an id (number) and the name of the trackplan to apply
//          */
//         const std::vector<Module> & get_modules() const { return modules; }
//         std::vector<Module> & get_mutable_modules() { return modules; }
//         void set_modules(const std::vector<Module>& value) { this->modules = value; }

//         /**
//          * List of available trackplans
//          */
//         const std::vector<Trackplan> & get_trackplans() const { return trackplans; }
//         std::vector<Trackplan> & get_mutable_trackplans() { return trackplans; }
//         void set_trackplans(const std::vector<Trackplan>& value) { this->trackplans = value; }

//         const nlohmann::json & get_schema() const { return schema; }
//         nlohmann::json & get_mutable_schema() { return schema; }
//         void set_schema(const nlohmann::json& value) { this->schema = value; }
//     };
// }

// namespace nlohmann {
//     inline void from_json(const json& _j, DccModel
// ::From& _x) {
//         _x.set_from_module( _j.at("module").get<std::string>() );
//         _x.set_path( _j.at("path").get<int64_t>() );
//     }

//     inline void to_json(json& _j, const DccModel
// ::From& _x) {
//         _j = json::object();
//         _j["module"] = _x.get_from_module();
//         _j["path"] = _x.get_path();
//     }

//     inline void from_json(const json& _j, DccModel
// ::To& _x) {
//         _x.set_to_module( _j.at("module").get<std::string>() );
//         _x.set_path( _j.at("path").get<int64_t>() );
//     }

//     inline void to_json(json& _j, const DccModel
// ::To& _x) {
//         _j = json::object();
//         _j["module"] = _x.get_to_module();
//         _j["path"] = _x.get_path();
//     }

//     inline void from_json(const json& _j, DccModel
// ::Junction& _x) {
//         _x.set_from( _j.at("from").get<DccModel
//     ::From>() );
//         _x.set_to( _j.at("to").get<DccModel
//     ::To>() );
//     }

//     inline void to_json(json& _j, const DccModel
// ::Junction& _x) {
//         _j = json::object();
//         _j["from"] = _x.get_from();
//         _j["to"] = _x.get_to();
//     }

//     inline void from_json(const json& _j, DccModel
// ::Layout& _x) {
//         _x.set_author( DccModel
//     ::get_optional<std::string>(_j, "author") );
//         _x.set_description( DccModel
//     ::get_optional<std::string>(_j, "description") );
//         _x.set_name( _j.at("name").get<std::string>() );
//         _x.set_version( _j.at("version").get<std::string>() );
//     }

//     inline void to_json(json& _j, const DccModel
// ::Layout& _x) {
//         _j = json::object();
//         _j["author"] = _x.get_author();
//         _j["description"] = _x.get_description();
//         _j["name"] = _x.get_name();
//         _j["version"] = _x.get_version();
//     }

//     inline void from_json(const json& _j, DccModel
// ::Module& _x) {
//         _x.set_description( DccModel
//     ::get_optional<std::string>(_j, "description") );
//         _x.set_name( _j.at("name").get<std::string>() );
//         _x.set_trackplan( _j.at("trackplan").get<std::string>() );
//     }

//     inline void to_json(json& _j, const DccModel
// ::Module& _x) {
//         _j = json::object();
//         _j["description"] = _x.get_description();
//         _j["name"] = _x.get_name();
//         _j["trackplan"] = _x.get_trackplan();
//     }

//     inline void from_json(const json& _j, DccModel
// ::Bumper& _x) {
//         _x.set_description( DccModel
//     ::get_optional<std::string>(_j, "description") );
//         _x.set_path( _j.at("path").get<int64_t>() );
//     }

//     inline void to_json(json& _j, const DccModel
// ::Bumper& _x) {
//         _j = json::object();
//         _j["description"] = _x.get_description();
//         _j["path"] = _x.get_path();
//     }

//     inline void from_json(const json& _j, DccModel
// ::Crossing& _x) {
//         _x.set_description( DccModel
//     ::get_optional<std::string>(_j, "description") );
//         _x.set_length( _j.at("length").get<int64_t>() );
//         _x.set_path( _j.at("path").get<std::vector<int64_t>>() );
//         _x.set_slip( _j.at("slip").get<std::vector<int64_t>>() );
//     }

//     inline void to_json(json& _j, const DccModel
// ::Crossing& _x) {
//         _j = json::object();
//         _j["description"] = _x.get_description();
//         _j["length"] = _x.get_length();
//         _j["path"] = _x.get_path();
//         _j["slip"] = _x.get_slip();
//     }

//     inline void from_json(const json& _j, DccModel
// ::Rail& _x) {
//         _x.set_description( DccModel
//     ::get_optional<std::string>(_j, "description") );
//         _x.set_length( _j.at("length").get<int64_t>() );
//         _x.set_path( _j.at("path").get<std::vector<int64_t>>() );
//     }

//     inline void to_json(json& _j, const DccModel
// ::Rail& _x) {
//         _j = json::object();
//         _j["description"] = _x.get_description();
//         _j["length"] = _x.get_length();
//         _j["path"] = _x.get_path();
//     }

//     inline void from_json(const json& _j, DccModel
// ::Turnout& _x) {
//         _x.set_description( DccModel
//     ::get_optional<std::string>(_j, "description") );
//         _x.set_length( _j.at("length").get<int64_t>() );
//         _x.set_narrow( _j.at("narrow").get<int64_t>() );
//         _x.set_wide( _j.at("wide").get<std::vector<int64_t>>() );
//     }

//     inline void to_json(json& _j, const DccModel
// ::Turnout& _x) {
//         _j = json::object();
//         _j["description"] = _x.get_description();
//         _j["length"] = _x.get_length();
//         _j["narrow"] = _x.get_narrow();
//         _j["wide"] = _x.get_wide();
//     }

//     inline void from_json(const json& _j, DccModel
// ::Trackelement& _x) {
//         _x.set_bumper( DccModel
//     ::get_optional<DccModel
//     ::Bumper>(_j, "bumper") );
//         _x.set_rail( DccModel
//     ::get_optional<DccModel
//     ::Rail>(_j, "rail") );
//         _x.set_turnout( DccModel
//     ::get_optional<DccModel
//     ::Turnout>(_j, "turnout") );
//         _x.set_crossing( DccModel
//     ::get_optional<DccModel
//     ::Crossing>(_j, "crossing") );
//     }

//     inline void to_json(json& _j, const DccModel
// ::Trackelement& _x) {
//         _j = json::object();
//         _j["bumper"] = _x.get_bumper();
//         _j["rail"] = _x.get_rail();
//         _j["turnout"] = _x.get_turnout();
//         _j["crossing"] = _x.get_crossing();
//     }

//     inline void from_json(const json& _j, DccModel
// ::Tracksection& _x) {
//         _x.set_name( _j.at("name").get<std::string>() );
//         _x.set_trackelements( _j.at("trackelements").get<std::vector<DccModel
//     ::Trackelement>>() );
//     }

//     inline void to_json(json& _j, const DccModel
// ::Tracksection& _x) {
//         _j = json::object();
//         _j["name"] = _x.get_name();
//         _j["trackelements"] = _x.get_trackelements();
//     }

//     inline void from_json(const json& _j, DccModel
// ::Trackplan& _x) {
//         _x.set_description( DccModel
//     ::get_optional<std::string>(_j, "description") );
//         _x.set_name( _j.at("name").get<std::string>() );
//         _x.set_tracksections( _j.at("tracksections").get<std::vector<DccModel
//     ::Tracksection>>() );
//     }

//     inline void to_json(json& _j, const DccModel
// ::Trackplan& _x) {
//         _j = json::object();
//         _j["description"] = _x.get_description();
//         _j["name"] = _x.get_name();
//         _j["tracksections"] = _x.get_tracksections();
//     }

//     inline void from_json(const json& _j, DccModel
// ::DccExLayout& _x) {
//         _x.set_junctions( DccModel
//     ::get_optional<std::vector<DccModel
//     ::Junction>>(_j, "junctions") );
//         _x.set_layout( _j.at("layout").get<DccModel
//     ::Layout>() );
//         _x.set_modules( _j.at("modules").get<std::vector<DccModel
//     ::Module>>() );
//         _x.set_trackplans( _j.at("trackplans").get<std::vector<DccModel
//     ::Trackplan>>() );
//         _x.set_schema( DccModel
//     ::get_untyped(_j, "$schema") );
//     }

//     inline void to_json(json& _j, const DccModel
// ::DccExLayout& _x) {
//         _j = json::object();
//         _j["junctions"] = _x.get_junctions();
//         _j["layout"] = _x.get_layout();
//         _j["modules"] = _x.get_modules();
//         _j["trackplans"] = _x.get_trackplans();
//         _j["$schema"] = _x.get_schema();
//     }
// }
