#include "ostream_dump.h"

std::ostream &parallel::print_format_json::operator<<(std::ostream &os, Plan &p) {
    return os << "{ \"imageWidth\": " << p.imageWidth()
              << ", \"imageHeight\": " << p.imageHeight()
              << ", \"cores\": " << p.cores()
              << ", \"sap\": " << p.sap()
              << ", \"baseRegions\": " << p.baseRegions()
              << " }";
}

std::ostream &parallel::print_format_json::operator<<(std::ostream &os, RegionSpec &rs) {
    os << "{ \"coresH\": " << rs.coresH()
       << ", \"coresV\": " << rs.coresV()
       << ", \"excessCores\": " << rs.excessCores()
       << ", \"data\": [ ";
    std::vector<Rect> &d = rs.data();
    for (auto t = d.begin(); t != d.end(); ++t) {
        os << *t;
        if (std::next(t) != d.end()) os << ", ";
    }
    return os << " ] }";
}

std::ostream &parallel::print_format_json::operator<<(std::ostream &os, const Rect &r) {
    return os << "{ \"x\": " << r.x << ", \"y\": " << r.y
              << ", \"w\": " << r.w << ", \"h\": " << r.h << " }";
}
