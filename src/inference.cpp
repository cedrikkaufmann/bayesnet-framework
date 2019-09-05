/*  This file is part of libBayesNet
 *
 *  Copyright (c) 2019, The libBayesNet authors. All rights reserved.
 */


#include <fstream>

#include <bayesnet/inference.h>
#include <bayesnet/exception.h>

#include <dai/bp.h>
#include <dai/cbp.h>
#include <dai/fbp.h>
#include <dai/jtree.h>


namespace bayesNet {

    namespace inference {

        Algorithm::Algorithm() : _algorithm(LOOPY_BELIEF_PROPAGATION),
                                 _inferenceProperties(DEFAULT_LOOPY_BELIEF_PROPAGATION_PROPERTIES),
                                 _inferenceInstance(NULL) {}

        Algorithm::Algorithm(const Algorithm::Type &alg, const std::string &properties) : _algorithm(alg),
                                                                                        _inferenceProperties(
                                                                                                properties),
                                                                                        _inferenceInstance(NULL) {}

        Algorithm::Algorithm(const std::string &filename) : _inferenceInstance(NULL) {
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
                    BAYESNET_THROW(INVALID_ALGORITHM_FILE);
                }

                _inferenceProperties = dai::PropertySet(inferenceAlgorithm);
                _filename = filename;
            } else {
                BAYESNET_THROW(INVALID_ALGORITHM_FILE);
            }
        }

        Algorithm::~Algorithm() {

        }

        void Algorithm::generateInferenceInstance(dai::FactorGraph &fg) {
            switch (_algorithm) {
                case Algorithm::LOOPY_BELIEF_PROPAGATION: {
                    _inferenceInstance = new dai::BP(fg, _inferenceProperties);
                    break;
                }

                case Algorithm::CONDITIONED_BELIEF_PROPAGATION: {
                    _inferenceInstance = new dai::CBP(fg, _inferenceProperties);
                    break;
                }

                case Algorithm::FRACTIONAL_BELIEF_PROPAGATION: {
                    _inferenceInstance = new dai::FBP(fg, _inferenceProperties);
                    break;
                }

                case Algorithm::JUNCTION_TREE: {
                    _inferenceInstance = new dai::JTree(fg, _inferenceProperties);
                    break;
                }
            }
        }

        void Algorithm::save(const std::string &filename) {
            std::ofstream file(filename);

            if (file.is_open()) {
                file << *this;
                file.close();

                _filename = filename;
            } else {
                BAYESNET_THROW(UNABLE_TO_WRITE_FILE);
            }
        }

        void Algorithm::save() {
            save(_filename);
        }

        dai::InfAlg *Algorithm::getInstance() {
            return _inferenceInstance;
        }

        Algorithm::Type Algorithm::getType() const {
            return _algorithm;
        }

        dai::PropertySet Algorithm::getProperties() const {
            return _inferenceProperties;
        }

        dai::PropertySet &Algorithm::getProperties() {
            return _inferenceProperties;
        }

        std::string Algorithm::getFilename() const {
            return _filename;
        }

        std::ostream &operator<<(std::ostream &os, const Algorithm &algorithm) {
            switch (algorithm.getType()) {
                case Algorithm::LOOPY_BELIEF_PROPAGATION: {
                    os << "BP" << std::endl;
                    break;
                }

                case Algorithm::CONDITIONED_BELIEF_PROPAGATION: {
                    os << "CBP" << std::endl;
                    break;
                }

                case Algorithm::FRACTIONAL_BELIEF_PROPAGATION: {
                    os << "FBP" << std::endl;
                    break;
                }

                case Algorithm::JUNCTION_TREE: {
                    os << "JT" << std::endl;
                    break;
                }
            }

            os << algorithm.getProperties();

            return os;
        }
    }
}
