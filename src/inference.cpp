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

        Algorithm::Algorithm(size_t type) : _algorithm(type), _inferenceInstance(NULL) {
            switch (type) {
                case Algorithm::LOOPY_BELIEF_PROPAGATION: {
                    _inferenceProperties = dai::PropertySet(DEFAULT_LOOPY_BELIEF_PROPAGATION_PROPERTIES);
                    break;
                }

                case Algorithm::CONDITIONED_BELIEF_PROPAGATION: {
                    _inferenceProperties = dai::PropertySet(DEFAULT_CONDITIONED_BELIEF_PROPAGATION_PROPERTIES);
                    break;
                }

                case Algorithm::FRACTIONAL_BELIEF_PROPAGATION: {
                    _inferenceProperties = dai::PropertySet(DEFAULT_FRACTIONAL_BELIEF_PROPAGATION_PROPERTIES);
                    break;
                }

                case Algorithm::JUNCTION_TREE: {
                    _inferenceProperties = dai::PropertySet(DEFAULT_JUNCTION_TREE_PROPERTIES);
                    break;
                }

                default:
                    BAYESNET_THROW(INDEX_OUT_OF_BOUNDS);
            }
        }

        Algorithm::Algorithm(size_t type, const std::string &properties) : _inferenceProperties(properties), _inferenceInstance(NULL) {
            if (type >= NUM_TYPES) {
                BAYESNET_THROW(INDEX_OUT_OF_BOUNDS);
            }

            _algorithm = type;
        }

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
            if (_inferenceInstance != NULL) {
                delete _inferenceInstance;
            }
        }

        void Algorithm::init(const std::vector<Node *> &nodes) {
            // collect factors
            std::vector<dai::Factor> factors;

            for (size_t i = 0; i < nodes.size(); ++i) {
                factors.push_back(nodes[i]->getFactor());
            }

            // create factor graph instance
            dai::FactorGraph fg(factors);

            // lookup factor graph index
            for (size_t i = 0; i < nodes.size(); ++i) {
                size_t factorIndex = fg.findFactor(nodes[i]->getConditionalDiscrete());
                nodes[i]->setFactorGraphIndex(factorIndex);
            }

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

        void Algorithm::init(Node &node) {
            if (_inferenceInstance == NULL) {
                BAYESNET_THROW(ALGORITHM_NOT_INITIALIZED);
            }

            _inferenceInstance->fg().setFactor(node.getFactorGraphIndex(), node.getFactor());
            _inferenceInstance->init(node.getConditionalDiscrete());
        }

        void Algorithm::save(const std::string &filename) {
            std::ofstream file(filename);

            if (file.is_open()) {
                _filename = filename;

                switch (_algorithm) {
                    case Algorithm::LOOPY_BELIEF_PROPAGATION: {
                        file << "BP" << std::endl;
                        break;
                    }

                    case Algorithm::CONDITIONED_BELIEF_PROPAGATION: {
                        file << "CBP" << std::endl;
                        break;
                    }

                    case Algorithm::FRACTIONAL_BELIEF_PROPAGATION: {
                        file << "FBP" << std::endl;
                        break;
                    }

                    case Algorithm::JUNCTION_TREE: {
                        file << "JT" << std::endl;
                        break;
                    }
                }

                file << _inferenceProperties;

                file.close();
            } else {
                BAYESNET_THROW(UNABLE_TO_WRITE_FILE);
            }
        }

        void Algorithm::save() {
            save(_filename);
        }

        size_t Algorithm::getType() const {
            return _algorithm;
        }

        dai::PropertySet Algorithm::getProperties() const {
            return _inferenceProperties;
        }

        dai::PropertySet &Algorithm::getProperties() {
            return _inferenceProperties;
        }

        const std::string &Algorithm::getFilename() const {
            return _filename;
        }

        void Algorithm::run() {
            if (_inferenceInstance == NULL) {
                BAYESNET_THROW(ALGORITHM_NOT_INITIALIZED);
            }

            _inferenceInstance->run();
        }

        state::BayesBelief Algorithm::belief(const Node &node) {
            if (_inferenceInstance == NULL) {
                BAYESNET_THROW(ALGORITHM_NOT_INITIALIZED);
            }

            dai::Factor belief = _inferenceInstance->belief(node.getDiscrete());
            state::BayesBelief bayesBelief(node.isBinary());

            for (size_t i = 0; i < belief.nrStates(); ++i) {
                bayesBelief[i] = belief[i];
            }

            return bayesBelief;
        }
    }
}
