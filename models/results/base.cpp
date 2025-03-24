//
// Created by Orill on 3/23/2025.
//

#ifndef BASE_CPP
#define BASE_CPP

#include "base.h"

namespace DosboxStagingReplacer {

    std::any BaseResult::fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) {
        throw BaseResultException("Method not implemented");
    }
} // DosboxStagingReplacer

#endif // BASE_CPP