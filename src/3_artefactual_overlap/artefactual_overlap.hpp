#ifndef ARTEFACTUAL_OVERLAP_H_
#define ARTEFACTUAL_OVERLAP_H_

#include "../overlap.hpp"

#include <regex>

const std::regex RGX_ACCESSION_DELIMITER{"[;-]"};

namespace artefactual_overlap {
void doAnalysis(const std::string& path_file_gene_search,
                const std::string& path_file_protein_search,
                const std::string& path_file_proteoform_search,
                const std::string& report_file_path);
}

#endif /* ARTEFACTUAL_OVERLAP_H_ */