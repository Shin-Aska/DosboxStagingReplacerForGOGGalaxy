//
// Created by Orill on 3/24/2025.
//

#ifndef PRODUCTDETAILS_H
#define PRODUCTDETAILS_H

#include "base.cpp"

namespace DosboxStagingReplacer {

    class ProductDetails: public BaseResult {
        using BaseResult::BaseResult;
    public:
        std::string productId;
        std::string title;
        std::string slug;
        std::string gogId;
        std::string releaseKey;
        std::any fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) override;
    };

} // DosboxStagingReplacer

#endif //PRODUCTDETAILS_H
