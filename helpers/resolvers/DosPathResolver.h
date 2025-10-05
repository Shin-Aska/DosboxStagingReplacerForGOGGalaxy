#ifndef DOSBOXSTAGINGREPLACER_DOSPATHRESOLVER_H
#define DOSBOXSTAGINGREPLACER_DOSPATHRESOLVER_H

#include <filesystem>
#include <string>
#include <vector>

namespace DosboxStagingReplacer {

    class DosPathResolver {
    public:
        static std::string resolveMountLinePaths(const std::string &line,
                                                 const std::filesystem::path &basePath);

        static std::filesystem::path clampRelativeToBase(const std::filesystem::path &base,
                                                         const std::filesystem::path &relative);

        static std::string restoreLeadingIndent(const std::string &original, const std::string &processed);
        /**
         * @brief Sanitizes the given DOSBox mount path.
         *
         * This function processes the provided path to ensure it meets the
         * requirements or conventions expected for DOSBox mount paths.
         *
         * @param path The DOSBox mount path to be sanitized.
         * @return A sanitized version of the given mount path.
         */
        static std::string sanitizeDosboxMountPath(const std::string &path);

    private:
        static std::string toLowerCopy(const std::string &value);
        static bool tokenIsOption(const std::string &token);
        static bool isQuotedToken(const std::string &token, char *quoteCharOut = nullptr);
        static std::string stripQuotationMarks(const std::string &token);
        static std::vector<std::string> splitDosboxArgs(const std::string &line);
        static std::string joinDosboxArgs(const std::vector<std::string> &tokens);
        static std::size_t findMountCommandIndex(const std::vector<std::string> &tokens);
        static bool shouldTreatAsPathToken(const std::vector<std::string> &tokens,
                                           std::size_t commandIdx,
                                           std::size_t idx);
    };

} // namespace DosboxStagingReplacer

#endif // DOSBOXSTAGINGREPLACER_DOSPATHRESOLVER_H
