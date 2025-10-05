#ifndef DOSBOXSTAGINGREPLACER_DOSPATHRESOLVER_H
#define DOSBOXSTAGINGREPLACER_DOSPATHRESOLVER_H

#include <filesystem>
#include <string>
#include <vector>

namespace DosboxStagingReplacer {

    class DosPathResolver {
    public:
        /**
         * @brief Resolve relative path arguments in a DOSBox mount command line.
         *
         * Splits the provided line into DOSBox arguments while respecting quotes,
         * then transforms each relative path component into an absolute path based
         * on the supplied base path. Non-path arguments and already absolute paths
         * are left untouched.
         *
         * @param line The raw mount or imgmount command line to process.
         * @param basePath The directory against which relative paths are resolved.
         * @return A command line with relative path tokens rewritten as absolute paths.
         */
        static std::string resolveMountLinePaths(const std::string &line,
                                                 const std::filesystem::path &basePath);

        /**
         * @brief Clamp a relative path so it never traverses above the given base path.
         *
         * Iterates over each component of the relative path, removing current-directory
         * markers and cancelling out parent-directory markers where possible. Remaining
         * segments are appended to the base path in order.
         *
         * @param base The root directory that provides the anchor for resolution.
         * @param relative The relative path expressed as a filesystem path.
         * @return The resolved filesystem path rooted at @p base.
         */
        static std::filesystem::path clampRelativeToBase(const std::filesystem::path &base,
                                                         const std::filesystem::path &relative);

        /**
         * @brief Restore the indentation from the original line to a processed line.
         *
         * Re-applies the leading whitespace prefix extracted from the original string
         * to the processed output. Useful when command lines must maintain their
         * formatting in configuration files.
         *
         * @param original The original line containing the desired indentation.
         * @param processed The line after content transformation, without indentation.
         * @return The processed string prefixed with the original leading whitespace.
         */
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
        /**
         * @brief Create a lowercase copy of the provided string.
         *
         * @param value The string to convert.
         * @return A lowercase version of @p value.
         */
        static std::string toLowerCopy(const std::string &value);
        /**
         * @brief Determine whether a token represents a command-line option.
         *
         * @param token The argument token to inspect.
         * @return True if the token starts with '-'; false otherwise.
         */
        static bool tokenIsOption(const std::string &token);
        /**
         * @brief Check if a token is quoted and optionally return the quote character.
         *
         * @param token The argument token to inspect.
         * @param quoteCharOut Optional pointer receiving the detected quote character.
         * @return True if the token is wrapped in matching quotes; false otherwise.
         */
        static bool isQuotedToken(const std::string &token, char *quoteCharOut = nullptr);
        /**
         * @brief Remove surrounding quotes from a token if present.
         *
         * @param token The argument token to unquote.
         * @return The token contents without the outer quotes.
         */
        static std::string stripQuotationMarks(const std::string &token);
        /**
         * @brief Split a command line into tokens while preserving quoted substrings.
         *
         * @param line The raw command line to tokenize.
         * @return A vector of tokens in their original order.
         */
        static std::vector<std::string> splitDosboxArgs(const std::string &line);
        /**
         * @brief Join tokens back into a single command-line string separated by spaces.
         *
         * @param tokens The argument tokens to join.
         * @return The space-delimited command-line string.
         */
        static std::string joinDosboxArgs(const std::vector<std::string> &tokens);
        /**
         * @brief Locate the index of the DOSBox mount command within tokenized arguments.
         *
         * @param tokens The tokenized command line.
         * @return The index of "mount" or "imgmount"; tokens.size() if not found.
         */
        static std::size_t findMountCommandIndex(const std::vector<std::string> &tokens);
        /**
         * @brief Determine if a token should be treated as a candidate path argument.
         *
         * @param tokens The tokenized command line.
         * @param commandIdx The index of the mount command token.
         * @param idx The index of the token under consideration.
         * @return True if the token represents a path argument; false otherwise.
         */
        static bool shouldTreatAsPathToken(const std::vector<std::string> &tokens,
                                           std::size_t commandIdx,
                                           std::size_t idx);
    };

} // namespace DosboxStagingReplacer

#endif // DOSBOXSTAGINGREPLACER_DOSPATHRESOLVER_H
