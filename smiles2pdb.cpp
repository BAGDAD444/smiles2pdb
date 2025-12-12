#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include <GraphMol/GraphMol.h>
#include <GraphMol/SmilesParse/SmilesParse.h>
#include <GraphMol/FileParsers/FileWriters.h>
#include <GraphMol/DistGeomHelpers/Embedder.h>
#include <GraphMol/ForceFieldHelpers/UFF/UFF.h>
#include <GraphMol/MolOps.h>

void printUsage(const char* programName) {
    std::cerr << "Usage: " << programName << " <SMILES_string> <output_file.pdb> [num_conformers]" << std::endl;
    std::cerr << "  num_conformers: Number of conformers to generate (default: 1)" << std::endl;
}

int main(int argc, char* argv[]) {
    // Check command line arguments
    if (argc < 3 || argc > 4) {
        printUsage(argv[0]);
        return 1;
    }

    // Parse the arguments: 1 is smiles, 2 is output pdb, 3 is optional num_conformers
    std::string smilesString = argv[1];
    std::string outputFile = argv[2];
    int numConformers = 1;

    // If num_conformers was defined, overwritte `numConformers`
    if (argc == 4) {
        try {
            numConformers = std::stoi(argv[3]);
            if (numConformers < 1) {
                std::cerr << "Error: Number of conformers must be at least 1" << std::endl;
                return 1;
            }
        } catch (...) {
            std::cerr << "Error: Invalid number of conformers" << std::endl;
            return 1;
        }
    }

    try {
        std::cout << "Parsing SMILES: " << smilesString << std::endl;

        // Step 1: Parse SMILES string
        std::unique_ptr<RDKit::RWMol> mol(RDKit::SmilesToMol(smilesString));
        if (!mol) {
            // TODO: Print the error
            std::cerr << "Error: Failed to parse SMILES string" << std::endl;
            return 1;
        }

        std::cout << "Molecule has "
                  << mol->getNumAtoms() << " atoms" << std::endl;

        // Step 2: Add hydrogens
        std::cout << "Adding hydrogens..." << std::endl;
        RDKit::MolOps::addHs(*mol);
        std::cout << "Molecule now has " << mol->getNumAtoms()
                  << " atoms " << std::endl;

        // Step 3: Generate 3D coordinates using ETKDG method
        // TODO: Add other methods?
        std::cout << "Generating " << numConformers << " conformer(s)..." << std::endl;
        RDKit::DGeomHelpers::EmbedParameters params(RDKit::DGeomHelpers::ETKDG);
        params.randomSeed = 42;
        params.numThreads = 0; // Use all available threads

        // Define a vector to hold the conformations
        std::vector<int> confIds;
        if (numConformers == 1) {
            // Generate just one conformation
            int confId = RDKit::DGeomHelpers::EmbedMolecule(*mol, params);
            if (confId < 0) {
                std::cerr << "Error: Failed to generate 3D coordinates" << std::endl;
                return 1;
            }
            // Add to the vector
            confIds.push_back(confId);
        } else {
            // Generate multiple conformations 
            //  - this method outputs a vector
            confIds = RDKit::DGeomHelpers::EmbedMultipleConfs(*mol, numConformers, params);
            if (confIds.empty()) {
                std::cerr << "Error: Failed to generate conformers" << std::endl;
                return 1;
            }
        }

        std::cout << "Successfully generated " << confIds.size() << " conformer(s)" << std::endl;

        // Step 4: Optimize geometry using UFF force field for all conformers
        std::cout << "Optimizing " << confIds.size() << " conformer(s) with UFF force field..." << std::endl;
        for (size_t i = 0; i < confIds.size(); ++i) {
            // TODO: Use other forcefields?
            // TODO: Make the number of iterations a parameter
            auto optimizationResult = RDKit::UFF::UFFOptimizeMolecule(*mol, 1000, confIds[i]);
            if (optimizationResult.first != 0) {
                std::cerr << "Warning: UFF optimization for conformer " << i+1
                          << " returned code " << optimizationResult.first << std::endl;
            }
        }
        std::cout << "Geometry optimization completed" << std::endl;

        // Step 5: Write all conformers to PDB file
        std::cout << "Writing " << confIds.size() << " conformer(s) to PDB file: " << outputFile << std::endl;
        std::ofstream outFile(outputFile);
        if (!outFile) {
            std::cerr << "Error: Could not open output file" << std::endl;
            return 1;
        }

        // Write each conformer as a separate MODEL in the PDB file
        for (size_t i = 0; i < confIds.size(); ++i) {
            if (confIds.size() > 1) {
                outFile << "MODEL     " << (i + 1) << std::endl;
            }
            std::string pdbBlock = RDKit::MolToPDBBlock(*mol, confIds[i]);
            outFile << pdbBlock;
            if (confIds.size() > 1) {
                outFile << "ENDMDL" << std::endl;
            }
        }
        outFile.close();

        std::cout << "Successfully created " << outputFile << " with "
                  << confIds.size() << " conformer(s)" << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}
