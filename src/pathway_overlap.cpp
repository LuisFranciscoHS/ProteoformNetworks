//============================================================================
// Description : Get pathway pairs that overlap only with modified proteins,
// 				 or that artefactually overlap at the gene or protein level
// Author      : Luis Francisco Hern�ndez S�nchez
// Copyright   : Licence Apache 2.0
//============================================================================

#include "pathway_overlap.h"

#include <cstdio>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <regex>
#include <string>

using namespace std;

const std::regex expression { "[;,]\\d{5}" };

bool isModified(const string& proteoform) {
	std::smatch modification;
	return std::regex_search(proteoform, modification, expression);
}

bool areAllModified(set<string> proteoforms) {
	for (const auto &proteoform : proteoforms) {
		if (!isModified(proteoform)) {
			return false;
		}
	}
	return true;
}

float calculateModifiedRatio(set<string> proteoforms) {
	int cont = 0;

	for (const auto &proteoform : proteoforms) {
		if (!isModified(proteoform)) {
			cont++;
		}
	}

	return (float) cont / (float) proteoforms.size();
}

//TODO: Find pairs of reactions that overlap only in modified proteoforms.

/**
 * Find key PTM examples: pairs of pathways that only share nodes that are
 * proteins with at least one modification
 */
set<pair<string, string>> findPairsWithKeyPTMExamples(
		float min_modified_percentage, string path_file_proteoform_search) {
	ifstream file_proteoform_search(path_file_proteoform_search);
	string field, pathway, proteoform;
	set<string> pathways; // Selected pathways to compare: at least one modified proteoform member
	multimap<string, string> pathways_to_proteoforms;
	set<pair<string, string>> result; // Insert always pairs ordering lexicographically the members

	getline(file_proteoform_search, field);	// Skip csv header line
	while (getline(file_proteoform_search, proteoform, '\t')) {	// Read the members of each pathway
		getline(file_proteoform_search, field, '\t'); 		// Read UNIPROT
		getline(file_proteoform_search, field, '\t');	// Read REACTION_STID
		getline(file_proteoform_search, field, '\t');// Read REACTION_DISPLAY_NAME
		getline(file_proteoform_search, pathway, '\t');		// Read PATHWAY_STID
		getline(file_proteoform_search, field);		// Read PATHWAY_DISPLAY_NAME

		if (isModified(proteoform)) {
			pathways.insert(pathway);
		}
		pathways_to_proteoforms.emplace(pathway, proteoform);

	}

	for (const auto &one_pathway : pathways) {
		set<string> one_set;
		auto ret = pathways_to_proteoforms.equal_range(one_pathway);
		for (auto it = ret.first; it != ret.second; ++it) {
			one_set.insert(it->second);
		}

		for (const auto &other_pathway : pathways) {
			if (one_pathway.compare(other_pathway) >= 0) {
				continue;
			}

//			cout << "Comparing " << one_pathway << " with " << other_pathway << "\n";

			set<string> other_set;
			ret = pathways_to_proteoforms.equal_range(other_pathway);
			for (auto it = ret.first; it != ret.second; ++it) {
				other_set.insert(it->second);
			}

			set<string> overlap;

			set_intersection(one_set.begin(), one_set.end(), other_set.begin(),
					other_set.end(), inserter(overlap, overlap.begin()));

			if (overlap.size() > 0) {
				if (calculateModifiedRatio(overlap) > min_modified_percentage) {
					result.emplace(one_pathway, other_pathway);

					cout << "===============================================\n";
					cout << one_pathway << " with " << other_pathway << "\n";
					cout << "-----------------------------------------------\n";
					for (auto const $proteoform : overlap) {
						cout << proteoform << "\t";
					}
					cout << "\n";
//
//					for (auto const $proteoform : one_set) {
//						cout << proteoform << "\n";
//					}
//					cout << "-----------------------------------------------\n";
//					for (auto const $proteoform : other_set) {
//						cout << proteoform << "\n";
//					}
					cout << "===============================================\n";
				}
			}
		}
	}

	return result;
}

/**
 * Find artefactual overlaps: pairs of pathways that share nodes only in the
 * gene or protein level, but not at the proteoform level
 */
set<pair<string, string>> findPathwayPairsWithArtifactualOverlapExamples(
		double minModifiedPercentage, string path_gene_search_file,
		string path_protein_search_file, string path_file_proteoform_search) {
	set<pair<string, string>> result; // Insert always pairs ordering lexicographically the members

	return result;
}