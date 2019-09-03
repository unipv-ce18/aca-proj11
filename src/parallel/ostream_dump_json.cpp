#include "ostream_dump.h"

std::ostream &parallel::print_format_json::operator<<(std::ostream &os, Plan &p) {
    return os << "{ \"imageWidth\": " << p.imageWidth()
              << ", \"imageHeight\": " << p.imageHeight()
              << ", \"cores\": " << p.cores()
              << ", \"sap\": " << p.sap()
              << ", \"baseRegions\": " << p.baseRegions()
              << ", \"effectiveRegions\": " << p.effectiveRegions()
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

std::ostream &parallel::print_format_json::operator<<(std::ostream &os, const CoreAllocation &ca) {
    os << "{ \"chunksH\": " << ca.chunksH()
       << ", \"chunksV\": " << ca.chunksV()
       << ", \"coresH\": " << ca.coresH()
       << ", \"coresV\": " << ca.coresV()
       << ", \"excessCores\": " << ca.excessCores()
       << ", \"allocation\": [ ";

    // TODO There is surely a better way w/ templates and overriding vector's printer
    // but I have no time to research on this cause stress, C++, time constraints, life, people, etc.
    const std::vector<std::vector<parallel::Chunk>> &alloc = ca.allocation();
    for (auto core = alloc.begin(); core != alloc.end(); ++core) {
        os << "[ ";
        for (auto chunk = core->begin(); chunk != core->end(); ++chunk) {
            os << *chunk;
            if (std::next(chunk) != core->end()) os << ", ";
        }
        os << " ]";

        if (std::next(core) != alloc.end()) os << ", ";
    }
    return os << " ] }";
}

std::ostream &parallel::print_format_json::operator<<(std::ostream &os, const Rect &r) {
    return os << "{ \"x\": " << r.x << ", \"y\": " << r.y
              << ", \"w\": " << r.w << ", \"h\": " << r.h << " }";
}

std::ostream &parallel::print_format_json::operator<<(std::ostream &os, const Chunk &c) {
    return os << "{ \"type\": " << c.type <<
                 ", \"x\": " << c.rect.x << ", \"y\": " << c.rect.y
              << ", \"w\": " << c.rect.w << ", \"h\": " << c.rect.h << " }";
}
