//
// Created by Cedrik Kaufmann on 2019-06-15.
//

#include <fstream>

#include <bayesnet/inference.h>
#include <bayesnet/exception.h>

#include <dai/bp.h>
#include <dai/cbp.h>
#include <dai/fbp.h>
#include <dai/jtree.h>

namespace bayesNet {

    namespace inference {

        void Algorithm::generateInferenceInstance(dai::FactorGraph &fg) {
            switch (_algorithm) {

                case inference::LOOPY_BELIEF_PROPAGATION: {
                    _inferenceInstance = new dai::BP(fg, _inferenceProperties);
                    break;
                }

                case inference::CONDITIONED_BELIEF_PROPAGATION: {
                    _inferenceInstance = new dai::CBP(fg, _inferenceProperties);
                    break;
                }

                case inference::FRACTIONAL_BELIEF_PROPAGATION: {
                    _inferenceInstance = new dai::FBP(fg, _inferenceProperties);
                    break;
                }
                case JUNCTION_TREE:
                    _inferenceInstance = new dai::JTree(fg, _inferenceProperties);
                    break;
            }
        }

        void Algorithm::save(const std::string &filename) {
            std::ofstream file(filename);

            if (file.is_open()) {

                file << *this;
                file.close();

                _filename = filename;
            } else {

                throw UnableWriteFileException();
            }
        }

        Algorithm::Algorithm(const std::string &filename) : _inferenceInstance(nullptr) {
            // load inference algorithm string from file
            std::ifstream inferenceAlgorithmFile(filename);
            std::string inferenceAlgorithm;
            std::string inferenceAlgorithmType;

            if (inferenceAlgorithmFile.is_open()) {

                // read algorithm type
                getline(inferenceAlgorithmFile, inferenceAlgorithmType);
                // read algorithm string
                getline(inferenceAlgorithmFile, inferenceAlgorithm);

                // close file descriptor
                inferenceAlgorithmFile.close();

                // generate inference algorithm from string
                if (inferenceAlgorithmType == "BP") {

                    _algorithm = LOOPY_BELIEF_PROPAGATION;
                } else if (inferenceAlgorithmType == "CBP") {

                    _algorithm = CONDITIONED_BELIEF_PROPAGATION;
                } else if (inferenceAlgorithmType == "FBP") {

                    _algorithm = FRACTIONAL_BELIEF_PROPAGATION;
                } else if (inferenceAlgorithmType == "JT") {

                    _algorithm = JUNCTION_TREE;
                } else {

                    throw InvalidAlgorithmFile();
                }

                _inferenceProperties = dai::PropertySet(inferenceAlgorithm);
                _filename = filename;
            } else {

                throw FileNotFoundException();
            }
        }

        std::ostream &operator<<(std::ostream &os, const Algorithm &algorithm) {

            switch (algorithm.getType()) {

                case inference::LOOPY_BELIEF_PROPAGATION: {
                    os << "BP" << std::endl;
                    break;
                }

                case inference::CONDITIONED_BELIEF_PROPAGATION: {
                    os << "CBP" << std::endl;
                    break;
                }

                case inference::FRACTIONAL_BELIEF_PROPAGATION: {
                    os << "FBP" << std::endl;
                    break;
                }

                case JUNCTION_TREE:
                    os << "JT" << std::endl;
                    break;
            }

            os << algorithm.getProperties();

            return os;
        }
    }
}
