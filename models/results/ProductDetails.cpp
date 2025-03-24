//
// Created by Orill on 3/24/2025.
//

#ifndef PRODUCTDETAILS_CPP
#define PRODUCTDETAILS_CPP

#include "ProductDetails.h"

namespace DosboxStagingReplacer {
    std::any ProductDetails::fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) {
        auto parser = StatementEngineParserFactory::createParser(engine);
        parser->parseInto(*this, parameters, stmt);
        return *this;
    }
} // DosboxStagingReplacer

#endif //PRODUCTDETAILS_CPP