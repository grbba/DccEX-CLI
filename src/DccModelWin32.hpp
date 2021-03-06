//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     DccModel::DccExLayout data = nlohmann::json::parse(jsonString);

#pragma once

#include <nlohmann/json.hpp>

#include <optional>
#include <stdexcept>
#include <regex>

#ifndef NLOHMANN_OPT_HELPER
#define NLOHMANN_OPT_HELPER
namespace nlohmann {
    template <typename T>
    struct adl_serializer<std::shared_ptr<T>> {
        static void to_json(json & j, std::shared_ptr<T> const & opt) {
            if (!opt) j = nullptr; else j = *opt;
        }

        static std::shared_ptr<T> from_json(json const & j) {
            if (j.is_null()) return std::unique_ptr<T>(); else return std::unique_ptr<T>(new T(j.get<T>()));
        }
    };
}
#endif

namespace DccModel {
    using nlohmann::json;

    inline json get_untyped(json const & j, char const * property) {
        if (j.find(property) != j.end()) {
            return j.at(property).get<json>();
        }
        return json();
    }

    inline json get_untyped(json const & j, std::string property) {
        return get_untyped(j, property.data());
    }

    template <typename T>
    inline std::shared_ptr<T> get_optional(json const & j, char const * property) {
        if (j.find(property) != j.end()) {
            return j.at(property).get<std::shared_ptr<T>>();
        }
        return std::shared_ptr<T>();
    }

    template <typename T>
    inline std::shared_ptr<T> get_optional(json const & j, std::string property) {
        return get_optional<T>(j, property.data());
    }


    class DccObject
    {

    public:
        DccObject() = default;
        virtual ~DccObject() = default;

    private:
        int64_t _dccid;

    public:
        int64_t get_dccid() const { return _dccid; }
        void set_dccid(int64_t value) { this->_dccid = value; }
    };

    /**
     * From connection
     */
    class From : public DccObject {
        public:
        From() = default;
        virtual ~From() = default;

        private:
        std::string from_module;
        int64_t path;

        public:
        /**
         * Module name of the path from which to connect
         */
        std::string const & get_from_module() const { return from_module; }
        std::string & get_mutable_from_module() { return from_module; }
        void set_from_module(std::string const & value) { this->from_module = value; }

        /**
         * Path id from which to connect
         */
        int64_t const & get_path() const { return path; }
        int64_t & get_mutable_path() { return path; }
        void set_path(int64_t const & value) { this->path = value; }
    };

    /**
     * To connection
     */
    class To : public DccObject {
        public:
        To() = default;
        virtual ~To() = default;

        private:
        std::string to_module;
        int64_t path;

        public:
        /**
         * Module name of the path to which to connect
         */
        std::string const & get_to_module() const { return to_module; }
        std::string & get_mutable_to_module() { return to_module; }
        void set_to_module(std::string const & value) { this->to_module = value; }

        /**
         * Path id to which to connect
         */
        int64_t const & get_path() const { return path; }
        int64_t & get_mutable_path() { return path; }
        void set_path(int64_t const & value) { this->path = value; }
    };

    /**
     * An entry in the junction list between to modules
     */
    class Junction  : public DccObject {
        public:
        Junction() = default;
        virtual ~Junction() = default;

        private:
        DccModel::From from;
        DccModel::To to;

        public:
        /**
         * From connection
         */
        DccModel::From const & get_from() const { return from; }
        DccModel::From & get_mutable_from() { return from; }
        void set_from(DccModel::From const & value) { this->from = value; }

        /**
         * To connection
         */
        DccModel::To const & get_to() const { return to; }
        DccModel::To & get_mutable_to() { return to; }
        void set_to(DccModel::To const & value) { this->to = value; }
    };

    /**
     * Description of the Layout incl. name of the layout a short text describing the layout and
     * the version. There can be only one layout per definition file
     */
    class Layout  : public DccObject {
        public:
        Layout() = default;
        virtual ~Layout() = default;

        private:
        std::shared_ptr<std::string> author;
        std::shared_ptr<std::string> description;
        std::string name;
        std::string version;

        public:
        /**
         * Author of this layout (optional)
         */
        std::shared_ptr<std::string> get_author() const { return author; }
        void set_author(std::shared_ptr<std::string> value) { this->author = value; }

        /**
         * A short description of the layout (optional)
         */
        std::shared_ptr<std::string> get_description() const { return description; }
        void set_description(std::shared_ptr<std::string> value) { this->description = value; }

        /**
         * Name of this layout
         */
        std::string const & get_name() const { return name; }
        std::string & get_mutable_name() { return name; }
        void set_name(std::string const & value) { this->name = value; }

        /**
         * Version of this layout definition in the form of x.y.z | x,y,z being numbers
         */
        std::string const & get_version() const { return version; }
        std::string & get_mutable_version() { return version; }
        void set_version(std::string const & value) { this->version = value; }
    };

    /**
     * A Module with a name, an id and a trackplan
     */
    class Module : public DccObject {
        public:
        Module() = default;
        virtual ~Module() = default;

        private:
        std::shared_ptr<std::string> description;
        std::string name;
        std::string trackplan;

        public:
        /**
         * Short description of the module (optional)
         */
        std::shared_ptr<std::string> get_description() const { return description; }
        void set_description(std::shared_ptr<std::string> value) { this->description = value; }

        /**
         * Name of the module
         */
        std::string const & get_name() const { return name; }
        std::string & get_mutable_name() { return name; }
        void set_name(std::string const & value) { this->name = value; }

        /**
         * Every module has a trackplan identified by its name.
         */
        std::string const & get_trackplan() const { return trackplan; }
        std::string & get_mutable_trackplan() { return trackplan; }
        void set_trackplan(std::string const & value) { this->trackplan = value; }
    };

    /**
     * End of a track closed by a bumper
     */
    class Bumper  : public DccObject{
        public:
        Bumper() = default;
        virtual ~Bumper() = default;

        private:
        std::shared_ptr<std::string> description;
        int64_t path;

        public:
        /**
         * Short text about the bumper e.g. 'End of refueling siding'.
         */
        std::shared_ptr<std::string> get_description() const { return description; }
        void set_description(std::shared_ptr<std::string> value) { this->description = value; }

        /**
         * The id of the bumper. Not a path per se as there is only one node id. The Id is used to
         * link to any other track element such as rail or Turnout
         */
        int64_t const & get_path() const { return path; }
        int64_t & get_mutable_path() { return path; }
        void set_path(int64_t const & value) { this->path = value; }
    };

    /**
     * Defines a crossing by means of four nodes; Length shall always be 0
     */
    class Crossing : public DccObject{
        public:
        Crossing() = default;
        virtual ~Crossing() = default;

        private:
        std::shared_ptr<std::string> description;
        int64_t length;
        std::vector<int64_t> path;
        std::vector<int64_t> slip;

        public:
        /**
         * Simple crossing, single or double slip
         */
        std::shared_ptr<std::string> get_description() const { return description; }
        void set_description(std::shared_ptr<std::string> value) { this->description = value; }

        /**
         * Length of the crossing. Should always be 0 as we can't reasonably store trains on the
         * crossings without blocking other routes/traffic
         */
        int64_t const & get_length() const { return length; }
        int64_t & get_mutable_length() { return length; }
        void set_length(int64_t const & value) { this->length = value; }

        /**
         * Nodes of the crossing specified in clock wise order starting top left.
         */
        std::vector<int64_t> const & get_path() const { return path; }
        std::vector<int64_t> & get_mutable_path() { return path; }
        void set_path(std::vector<int64_t> const & value) { this->path = value; }

        /**
         * Possible slip start or end points. The second slip point will be calculated automatically
         * from the crossing path nodes. If empty we have a simple crossing
         */
        std::vector<int64_t> const & get_slip() const { return slip; }
        std::vector<int64_t> & get_mutable_slip() { return slip; }
        void set_slip(std::vector<int64_t> const & value) { this->slip = value; }
    };

    /**
     * A piece of single track having two end points and nothing in between them.
     */
    class Rail  : public DccObject {
        public:
        Rail() = default;
        virtual ~Rail() = default;

        private:
        std::shared_ptr<std::string> description;
        int64_t length;
        std::vector<int64_t> path;

        public:
        /**
         * Short text about the track segment.
         */
        std::shared_ptr<std::string> get_description() const { return description; }
        void set_description(std::shared_ptr<std::string> value) { this->description = value; }

        /**
         * Length of the track segment
         */
        int64_t const & get_length() const { return length; }
        int64_t & get_mutable_length() { return length; }
        void set_length(int64_t const & value) { this->length = value; }

        /**
         * Array of two node id's representing a segment of track
         */
        std::vector<int64_t> const & get_path() const { return path; }
        std::vector<int64_t> & get_mutable_path() { return path; }
        void set_path(std::vector<int64_t> const & value) { this->path = value; }
    };

    /**
     * Defines a turnout by means of three nodes; 2 on the wide end and 1 on the narrow end.
     * Length shall always be 0
     */
    class Turnout : public DccObject {
        public:
        Turnout() = default;
        virtual ~Turnout() = default;

        private:
        std::shared_ptr<std::string> description;
        int64_t length;
        int64_t narrow;
        std::vector<int64_t> wide;

        public:
        /**
         * The narrow end of the turnout i.e. the part opposite to the two other end points ( wide
         * end )
         */
        std::shared_ptr<std::string> get_description() const { return description; }
        void set_description(std::shared_ptr<std::string> value) { this->description = value; }

        /**
         * Length of the turnout. Should always be 0 as we can't reasonably store trains on the
         * turnouts without blocking other routes/traffic
         */
        int64_t const & get_length() const { return length; }
        int64_t & get_mutable_length() { return length; }
        void set_length(int64_t const & value) { this->length = value; }

        /**
         * The narrow end of the turnout i.e. the part opposite to the two other end points ( wide
         * end ) represented by a node id (number)
         */
        int64_t const & get_narrow() const { return narrow; }
        int64_t & get_mutable_narrow() { return narrow; }
        void set_narrow(int64_t const & value) { this->narrow = value; }

        /**
         * The wide end of the turnout. i.e. the two end points opposite to the single other end
         * points (narrow end)
         */
        std::vector<int64_t> const & get_wide() const { return wide; }
        std::vector<int64_t> & get_mutable_wide() { return wide; }
        void set_wide(std::vector<int64_t> const & value) { this->wide = value; }
    };

    /**
     * Bumper
     *
     * Track segment
     *
     * A turnout with two or three connections
     *
     * A crossing with four connections
     */
    class Trackelement  : public DccObject {
        public:
        Trackelement() = default;
        virtual ~Trackelement() = default;

        private:
        std::shared_ptr<DccModel::Bumper> bumper;
        std::shared_ptr<DccModel::Rail> rail;
        std::shared_ptr<DccModel::Turnout> turnout;
        std::shared_ptr<DccModel::Crossing> crossing;

        public:
        /**
         * End of a track closed by a bumper
         */
        std::shared_ptr<DccModel::Bumper> get_bumper() const { return bumper; }
        void set_bumper(std::shared_ptr<DccModel::Bumper> value) { this->bumper = value; }

        /**
         * A piece of single track having two end points and nothing in between them.
         */
        std::shared_ptr<DccModel::Rail> get_rail() const { return rail; }
        void set_rail(std::shared_ptr<DccModel::Rail> value) { this->rail = value; }

        /**
         * Defines a turnout by means of three nodes; 2 on the wide end and 1 on the narrow end.
         * Length shall always be 0
         */
        std::shared_ptr<DccModel::Turnout> get_turnout() const { return turnout; }
        void set_turnout(std::shared_ptr<DccModel::Turnout> value) { this->turnout = value; }

        /**
         * Defines a crossing by means of four nodes; Length shall always be 0
         */
        std::shared_ptr<DccModel::Crossing> get_crossing() const { return crossing; }
        void set_crossing(std::shared_ptr<DccModel::Crossing> value) { this->crossing = value; }
    };

    /**
     * A track section is a collection of track elements ( bumper, turnout, rail) which are
     * connected to each other
     */
    class Tracksection  : public DccObject {
        public:
        Tracksection() = default;
        virtual ~Tracksection() = default;

        private:
        std::string name;
        std::vector<DccModel::Trackelement> trackelements;

        public:
        /**
         * Name of the track section e.g. main or industry etc...
         */
        std::string const & get_name() const { return name; }
        std::string & get_mutable_name() { return name; }
        void set_name(std::string const & value) { this->name = value; }

        /**
         * List of all the track elements of the track section (bumper, rail or turnout).
         */
        std::vector<DccModel::Trackelement> const & get_trackelements() const { return trackelements; }
        std::vector<DccModel::Trackelement> & get_mutable_trackelements() { return trackelements; }
        void set_trackelements(std::vector<DccModel::Trackelement> const & value) { this->trackelements = value; }
    };

    /**
     * A track plan is described by its name which is used in a module description and it's
     * track sections.
     */
    class Trackplan  : public DccObject {
        public:
        Trackplan() = default;
        virtual ~Trackplan() = default;

        private:
        std::shared_ptr<std::string> description;
        std::string name;
        std::vector<DccModel::Tracksection> tracksections;

        public:
        /**
         * Short text describing the track plan (optional)
         */
        std::shared_ptr<std::string> get_description() const { return description; }
        void set_description(std::shared_ptr<std::string> value) { this->description = value; }

        /**
         * Name of the track plan to be used at the module level
         */
        std::string const & get_name() const { return name; }
        std::string & get_mutable_name() { return name; }
        void set_name(std::string const & value) { this->name = value; }

        /**
         * List of track sections for this track plan. Track sections are user defined and represent
         * some logical design of the layout. iin its simplest form the full track plan has only one
         * track section
         */
        std::vector<DccModel::Tracksection> const & get_tracksections() const { return tracksections; }
        std::vector<DccModel::Tracksection> & get_mutable_tracksections() { return tracksections; }
        void set_tracksections(std::vector<DccModel::Tracksection> const & value) { this->tracksections = value; }
    };

    /**
     * The root DccLayout Schema comprises the entire JSON document i.e. provides an entry point
     * to acces to all layout elements
     */
    class DccExLayout {
        public:
        DccExLayout() = default;
        virtual ~DccExLayout() = default;

        private:
        std::shared_ptr<std::vector<DccModel::Junction>> junctions;
        DccModel::Layout layout;
        std::vector<DccModel::Module> modules;
        std::vector<DccModel::Trackplan> trackplans;
        json schema;

        public:
        /**
         * An array of junctions between modules
         */
        std::shared_ptr<std::vector<DccModel::Junction>> get_junctions() const { return junctions; }
        void set_junctions(std::shared_ptr<std::vector<DccModel::Junction>> value) { this->junctions = value; }

        /**
         * Description of the Layout incl. name of the layout a short text describing the layout and
         * the version. There can be only one layout per definition file
         */
        DccModel::Layout const & get_layout() const { return layout; }
        DccModel::Layout & get_mutable_layout() { return layout; }
        void set_layout(DccModel::Layout const & value) { this->layout = value; }

        /**
         * A Layout is composed by at least one or more physical modules. Modules are defined in an
         * array and have to have a name an id (number) and the name of the trackplan to apply
         */
        std::vector<DccModel::Module> const & get_modules() const { return modules; }
        std::vector<DccModel::Module> & get_mutable_modules() { return modules; }
        void set_modules(std::vector<DccModel::Module> const & value) { this->modules = value; }

        /**
         * List of available trackplans
         */
        std::vector<DccModel::Trackplan> const & get_trackplans() const { return trackplans; }
        std::vector<DccModel::Trackplan> & get_mutable_trackplans() { return trackplans; }
        void set_trackplans(std::vector<DccModel::Trackplan> const & value) { this->trackplans = value; }

        json const & get_schema() const { return schema; }
        json & get_mutable_schema() { return schema; }
        void set_schema(json const & value) { this->schema = value; }
    };
}

namespace nlohmann {
namespace detail {
    void from_json(json const & j, DccModel::From & x);
    void to_json(json & j, DccModel::From const & x);

    void from_json(json const & j, DccModel::To & x);
    void to_json(json & j, DccModel::To const & x);

    void from_json(json const & j, DccModel::Junction & x);
    void to_json(json & j, DccModel::Junction const & x);

    void from_json(json const & j, DccModel::Layout & x);
    void to_json(json & j, DccModel::Layout const & x);

    void from_json(json const & j, DccModel::Module & x);
    void to_json(json & j, DccModel::Module const & x);

    void from_json(json const & j, DccModel::Bumper & x);
    void to_json(json & j, DccModel::Bumper const & x);

    void from_json(json const & j, DccModel::Crossing & x);
    void to_json(json & j, DccModel::Crossing const & x);

    void from_json(json const & j, DccModel::Rail & x);
    void to_json(json & j, DccModel::Rail const & x);

    void from_json(json const & j, DccModel::Turnout & x);
    void to_json(json & j, DccModel::Turnout const & x);

    void from_json(json const & j, DccModel::Trackelement & x);
    void to_json(json & j, DccModel::Trackelement const & x);

    void from_json(json const & j, DccModel::Tracksection & x);
    void to_json(json & j, DccModel::Tracksection const & x);

    void from_json(json const & j, DccModel::Trackplan & x);
    void to_json(json & j, DccModel::Trackplan const & x);

    void from_json(json const & j, DccModel::DccExLayout & x);
    void to_json(json & j, DccModel::DccExLayout const & x);

    inline void from_json(json const & j, DccModel::From& x) {
        x.set_from_module(j.at("module").get<std::string>());
        x.set_path(j.at("path").get<int64_t>());
    }

    inline void to_json(json & j, DccModel::From const & x) {
        j = json::object();
        j["module"] = x.get_from_module();
        j["path"] = x.get_path();
    }

    inline void from_json(json const & j, DccModel::To& x) {
        x.set_to_module(j.at("module").get<std::string>());
        x.set_path(j.at("path").get<int64_t>());
    }

    inline void to_json(json & j, DccModel::To const & x) {
        j = json::object();
        j["module"] = x.get_to_module();
        j["path"] = x.get_path();
    }

    inline void from_json(json const & j, DccModel::Junction& x) {
        x.set_from(j.at("from").get<DccModel::From>());
        x.set_to(j.at("to").get<DccModel::To>());
    }

    inline void to_json(json & j, DccModel::Junction const & x) {
        j = json::object();
        j["from"] = x.get_from();
        j["to"] = x.get_to();
    }

    inline void from_json(json const & j, DccModel::Layout& x) {
        x.set_author(DccModel::get_optional<std::string>(j, "author"));
        x.set_description(DccModel::get_optional<std::string>(j, "description"));
        x.set_name(j.at("name").get<std::string>());
        x.set_version(j.at("version").get<std::string>());
    }

    inline void to_json(json & j, DccModel::Layout const & x) {
        j = json::object();
        j["author"] = x.get_author();
        j["description"] = x.get_description();
        j["name"] = x.get_name();
        j["version"] = x.get_version();
    }

    inline void from_json(json const & j, DccModel::Module& x) {
        x.set_description(DccModel::get_optional<std::string>(j, "description"));
        x.set_name(j.at("name").get<std::string>());
        x.set_trackplan(j.at("trackplan").get<std::string>());
    }

    inline void to_json(json & j, DccModel::Module const & x) {
        j = json::object();
        j["description"] = x.get_description();
        j["name"] = x.get_name();
        j["trackplan"] = x.get_trackplan();
    }

    inline void from_json(json const & j, DccModel::Bumper& x) {
        x.set_description(DccModel::get_optional<std::string>(j, "description"));
        x.set_path(j.at("path").get<int64_t>());
    }

    inline void to_json(json & j, DccModel::Bumper const & x) {
        j = json::object();
        j["description"] = x.get_description();
        j["path"] = x.get_path();
    }

    inline void from_json(json const & j, DccModel::Crossing& x) {
        x.set_description(DccModel::get_optional<std::string>(j, "description"));
        x.set_length(j.at("length").get<int64_t>());
        x.set_path(j.at("path").get<std::vector<int64_t>>());
        x.set_slip(j.at("slip").get<std::vector<int64_t>>());
    }

    inline void to_json(json & j, DccModel::Crossing const & x) {
        j = json::object();
        j["description"] = x.get_description();
        j["length"] = x.get_length();
        j["path"] = x.get_path();
        j["slip"] = x.get_slip();
    }

    inline void from_json(json const & j, DccModel::Rail& x) {
        x.set_description(DccModel::get_optional<std::string>(j, "description"));
        x.set_length(j.at("length").get<int64_t>());
        x.set_path(j.at("path").get<std::vector<int64_t>>());
    }

    inline void to_json(json & j, DccModel::Rail const & x) {
        j = json::object();
        j["description"] = x.get_description();
        j["length"] = x.get_length();
        j["path"] = x.get_path();
    }

    inline void from_json(json const & j, DccModel::Turnout& x) {
        x.set_description(DccModel::get_optional<std::string>(j, "description"));
        x.set_length(j.at("length").get<int64_t>());
        x.set_narrow(j.at("narrow").get<int64_t>());
        x.set_wide(j.at("wide").get<std::vector<int64_t>>());
    }

    inline void to_json(json & j, DccModel::Turnout const & x) {
        j = json::object();
        j["description"] = x.get_description();
        j["length"] = x.get_length();
        j["narrow"] = x.get_narrow();
        j["wide"] = x.get_wide();
    }

    inline void from_json(json const & j, DccModel::Trackelement& x) {
        x.set_bumper(DccModel::get_optional<DccModel::Bumper>(j, "bumper"));
        x.set_rail(DccModel::get_optional<DccModel::Rail>(j, "rail"));
        x.set_turnout(DccModel::get_optional<DccModel::Turnout>(j, "turnout"));
        x.set_crossing(DccModel::get_optional<DccModel::Crossing>(j, "crossing"));
    }

    inline void to_json(json & j, DccModel::Trackelement const & x) {
        j = json::object();
        j["bumper"] = x.get_bumper();
        j["rail"] = x.get_rail();
        j["turnout"] = x.get_turnout();
        j["crossing"] = x.get_crossing();
    }

    inline void from_json(json const & j, DccModel::Tracksection& x) {
        x.set_name(j.at("name").get<std::string>());
        x.set_trackelements(j.at("trackelements").get<std::vector<DccModel::Trackelement>>());
    }

    inline void to_json(json & j, DccModel::Tracksection const & x) {
        j = json::object();
        j["name"] = x.get_name();
        j["trackelements"] = x.get_trackelements();
    }

    inline void from_json(json const & j, DccModel::Trackplan& x) {
        x.set_description(DccModel::get_optional<std::string>(j, "description"));
        x.set_name(j.at("name").get<std::string>());
        x.set_tracksections(j.at("tracksections").get<std::vector<DccModel::Tracksection>>());
    }

    inline void to_json(json & j, DccModel::Trackplan const & x) {
        j = json::object();
        j["description"] = x.get_description();
        j["name"] = x.get_name();
        j["tracksections"] = x.get_tracksections();
    }

    inline void from_json(json const & j, DccModel::DccExLayout& x) {
        x.set_junctions(DccModel::get_optional<std::vector<DccModel::Junction>>(j, "junctions"));
        x.set_layout(j.at("layout").get<DccModel::Layout>());
        x.set_modules(j.at("modules").get<std::vector<DccModel::Module>>());
        x.set_trackplans(j.at("trackplans").get<std::vector<DccModel::Trackplan>>());
        x.set_schema(DccModel::get_untyped(j, "$schema"));
    }

    inline void to_json(json & j, DccModel::DccExLayout const & x) {
        j = json::object();
        j["junctions"] = x.get_junctions();
        j["layout"] = x.get_layout();
        j["modules"] = x.get_modules();
        j["trackplans"] = x.get_trackplans();
        j["$schema"] = x.get_schema();
    }
}
}
