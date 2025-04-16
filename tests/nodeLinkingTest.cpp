#include <gtest/gtest.h>
#include <i2/nodeLoader.hpp>
#include <algorithm>

// Consts used in multiple test functions
const std::string ERROR_INVALID_NODE = "Error: Invalid node.";
const std::string RESOURCE_PATH = "../resources/";
const std::string DATA_PATH = RESOURCE_PATH + "data.json";
const std::vector<unsigned int> EXPECTED_WEIGHT = {158,104,91,84,68,68,66,61,56,47,47,43,39,38,34,31,29,27,26,26,25,25,25,24,24,24,23,20,19,19,19,17,16,16,14,14,14,13,12,11,11,11,11,8,7,5,5,5,5,5,4,4,3,3,3,3,3,3,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

TEST(i2GroupUnitTest, ErrorIsThrownWhenLinkIsOutOfBounds)
{
    const std::string dataPath = RESOURCE_PATH + "invalidNodeIndex.json";
    const std::string expectedError = "Invalid link at index '2'.";

    std::string error = "";

    try
    {
        I2::NodeLoader::loadNodesFromFile(dataPath); // An error should be thrown when processing links, because an invalid index is specified in the link data
    }
    catch(const std::runtime_error &e)
    {
        error = e.what();
    }

    EXPECT_EQ(error,expectedError); // Testing outside of the catch, because if the error is not thrown then the catch will not be executed
}

TEST(i2GroupUnitTest, ErrorIsThrownWhenAddingNullLink)
{
    std::shared_ptr<I2::Node> node1 = std::make_shared<I2::Node>("Node1");
    std::string error = "";

    try
    {
        node1->addLink(nullptr, 1); // Should throw an error when the pointer to the node is null
    }
    catch(const std::runtime_error &e)
    {
        error = e.what();
    }

    EXPECT_EQ(error,ERROR_INVALID_NODE); // Testing outside of the catch, because if the error is not thrown then the catch will not be executed
}

TEST(i2GroupUnitTest, ErrorIsThrownWhenRemovingNullLink)
{
    std::shared_ptr<I2::Node> node1 = std::make_shared<I2::Node>("Node1");
    std::string error = "";

    try
    {
        node1->removeLink(nullptr); // Should throw an error when the pointer to the node is null
    }
    catch(const std::runtime_error &e)
    {
        error = e.what();
    }

    EXPECT_EQ(error,ERROR_INVALID_NODE); // Testing outside of the catch, because if the error is not thrown then the catch will not be executed
}

TEST(i2GroupUnitTest, ErrorIsThrownWhenNodeHasNoName)
{
    const std::string expectedError = "Node invalid: no name provided!";
    std::string error = "";

    try
    {
        std::make_unique<I2::Node>(""); // Should throw an error due to not having a name
    }
    catch(const std::runtime_error &e)
    {
        error = e.what();
    }

    EXPECT_EQ(error,expectedError); // Testing outside of the catch, because if the error is not thrown then the catch will not be executed
}

TEST(i2GroupUnitTest, ErrorIsThrownWhenSelfRefNotAccepted)
{
    const std::string dataPath = RESOURCE_PATH + "nodeSelfReference.json";
    const std::string expectedError = "Invalid link at index '0'.";

    std::string error = "";

    try
    {
        I2::NodeLoader::loadNodesFromFile(dataPath); // An error should be thrown when processing links, because allowing nodes to self link is not enabled
    }
    catch(const std::runtime_error &e)
    {
        error = e.what();
    }

    EXPECT_EQ(error,expectedError); // Testing outside of the catch, because if the error is not thrown then the catch will not be executed
}

TEST(i2GroupUnitTest, LoadsAllExpectedNodes)
{
    
    const std::string dataPath = RESOURCE_PATH + "data.json";
    const std::size_t expectedNodeCount = 77;

    std::vector<std::string> expectedName = {"Myriel","Napoleon","Mlle.Baptistine","Mme.Magloire","CountessdeLo","Geborand","Champtercier","Cravatte","Count","OldMan","Labarre","Valjean","Marguerite","Mme.deR","Isabeau","Gervais","Tholomyes","Listolier","Fameuil","Blacheville","Favourite","Dahlia","Zephine","Fantine","Mme.Thenardier","Thenardier","Cosette","Javert","Fauchelevent","Bamatabois","Perpetue","Simplice","Scaufflaire","Woman1","Judge","Champmathieu","Brevet","Chenildieu","Cochepaille","Pontmercy","Boulatruelle","Eponine","Anzelma","Woman2","MotherInnocent","Gribier","Jondrette","Mme.Burgon","Gavroche","Gillenormand","Magnon","Mlle.Gillenormand","Mme.Pontmercy","Mlle.Vaubois","Lt.Gillenormand","Marius","BaronessT","Mabeuf","Enjolras","Combeferre","Prouvaire","Feuilly","Courfeyrac","Bahorel","Bossuet","Joly","Grantaire","MotherPlutarch","Gueulemer","Babet","Claquesous","Montparnasse","Toussaint","Child1","Child2","Brujon","Mme.Hucheloup"};

    std::vector<std::shared_ptr<I2::Node>> nodeList;
    std::string name = "";
    
    EXPECT_NO_THROW(nodeList = std::move(I2::NodeLoader::loadNodesFromFile(dataPath))); // Should load fine without issues
    ASSERT_EQ(nodeList.size(),expectedNodeCount);

    // Checks we have no unexpected names
    // Once found: the name is removed from the expectedName list, so we can be certain all names present (by checking the list is empty after the loop)
    for(auto n : nodeList)
    {
        name = n->getName();
        auto itName = std::find(expectedName.begin(),expectedName.end(),name);

       ASSERT_NE(itName,expectedName.end()); // Ensure the name is not missing
       expectedName.erase(itName);
    }

    EXPECT_EQ(expectedName.size(),0); // Should be no names remaining if all expected names were loaded
}

TEST(i2GroupUnitTest, NodeWeightedDegreesAreAccurateOnInitialLoad)
{
    std::vector<std::shared_ptr<I2::Node>> nodeList;
    int nodeCount = 0;

    EXPECT_NO_THROW(nodeList = std::move(I2::NodeLoader::loadNodesFromFile(DATA_PATH))); // Should load fine without issues

    nodeCount = nodeList.size();
    ASSERT_EQ(nodeCount,EXPECTED_WEIGHT.size()); // If the size/count do not match, then we either have too many or too few nodes

    std::sort(nodeList.begin(),nodeList.end(),I2::nodeCompareGT); // Sort into descending order (by weighted degree)

    for(int i=0;i<nodeCount;++i)
        EXPECT_EQ(nodeList[i]->getWeightedDegree(),EXPECTED_WEIGHT[i]); // Compare each individual weighted degree to the respective value we expect
}

TEST(i2GroupUnitTest, NodeWeightedDegreeIsAccurateWhenCopied)
{ // Testing copy constructor and recalculateWeightedDegree
    std::vector<std::shared_ptr<I2::Node>> nodeList;
    std::shared_ptr<I2::Node> copyNode = nullptr;
    int nodeCount = 0;

    EXPECT_NO_THROW(nodeList = std::move(I2::NodeLoader::loadNodesFromFile(DATA_PATH))); // Should load fine without issues

    nodeCount = nodeList.size();
    ASSERT_EQ(nodeCount,EXPECTED_WEIGHT.size()); // If the size/count do not match, then we either have too many or too few nodes

    std::sort(nodeList.begin(),nodeList.end(),I2::nodeCompareGT); // Sort into descending order (by weighted degree)
    copyNode = std::make_shared<I2::Node>(*nodeList[0]); // Take a copy of the first node

    EXPECT_EQ(copyNode->getWeightedDegree(),EXPECTED_WEIGHT[0]); // Ensure the copy constructor and Node::recalculateWeightedDegree are working as expected

    for(int i=0;i<nodeCount;++i) // Ensure all other weighted degrees remain as expected
        EXPECT_EQ(nodeList[i]->getWeightedDegree(),EXPECTED_WEIGHT[i]);
}

TEST(i2GroupUnitTest, LinkIsRemovedSuccessfully)
{
    // Ensure link was successfully removed: ensure getLinkCount == previousLinkCount - 1, and ensure new weighted degree is as expected
    const unsigned long expectedLinkCount = 36, expectedInitWeight = 158;

    std::vector<std::shared_ptr<I2::Node>> nodeList;
    std::unordered_map<std::shared_ptr<I2::Node>, unsigned int> linkMap;
    std::unordered_map<std::shared_ptr<I2::Node>, unsigned int>::const_iterator itN;
    int nodeCount = 0;
    unsigned int linkCount = 0, expectedNewWeight = 0;

    EXPECT_NO_THROW(nodeList = std::move(I2::NodeLoader::loadNodesFromFile(DATA_PATH))); // Should load fine without issues
    nodeCount = nodeList.size();

    std::sort(nodeList.begin(),nodeList.end(),I2::nodeCompareGT); // Sort into descending order (by weighted degree)
    linkCount = nodeList[0]->getLinkCount();

    EXPECT_EQ(nodeList[0]->getWeightedDegree(),expectedInitWeight);
    ASSERT_EQ(linkCount,expectedLinkCount);

    linkMap = nodeList[0]->getLinks(); // Retrieve the links so we have a link to remove and know the expected differentiation in weighted degree
    itN = linkMap.cbegin(); // We already know the map is not empty, so there is no need to check again cend()

    expectedNewWeight = expectedInitWeight - itN->second;
    nodeList[0]->removeLink(itN->first);

    EXPECT_EQ(nodeList[0]->getLinkCount(),expectedLinkCount-1); // The link count should only have decreased by 1
    EXPECT_EQ(nodeList[0]->getWeightedDegree(),expectedNewWeight); // Ensure weighted degree has updated
}

TEST(i2GroupUnitTest, AddingDuplicateLinkFails)
{
    // Test both addLink and addLinks (with one pre-existing node in the map)
    const unsigned int expectedWeight = 2, expectedLinkCount = 1;
    std::shared_ptr<I2::Node> node1 = nullptr, node2 = nullptr;
    std::unordered_map<std::shared_ptr<I2::Node>, unsigned int> linkMap;

    node1 = std::make_shared<I2::Node>("Node1");
    node2 = std::make_shared<I2::Node>("Node2");

    linkMap.insert(std::pair<std::shared_ptr<I2::Node>,unsigned int>(node2,expectedWeight));

    // Should not contain any links at this stage
    EXPECT_EQ(node1->getWeightedDegree(),0);
    EXPECT_EQ(node1->getLinkCount(),0);

    node1->addLink(node2,expectedWeight);

    EXPECT_EQ(node1->getWeightedDegree(),expectedWeight);
    EXPECT_EQ(node1->getLinkCount(),expectedLinkCount);

    // Adding the same link again should fail
    node1->addLink(node2,expectedWeight);

    EXPECT_EQ(node1->getWeightedDegree(),expectedWeight); // Ensure the weighted degree remains the same
    EXPECT_EQ(node1->getLinkCount(),expectedLinkCount); // Ensure the link count remains the same

    // Ensure still fails when adding duplicate using a link map
    node1->addLinks(linkMap); 

    EXPECT_EQ(node1->getWeightedDegree(),expectedWeight); // Ensure the weighted degree remains the same
    EXPECT_EQ(node1->getLinkCount(),expectedLinkCount); // Ensure the link count remains the same
}

TEST(i2GroupUnitTest, PageRankIsComputedAccurately)
{
    const std::vector<double> expectedPageRank = {1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,952.29049010436199,868.28242387053172,661.11305916305912,486.29723171565269,452.74087579087569,441.90544078097997,425.00194805194803,338.07012987012985,244.446907539867,231.8201298701299,228.15851370851368,198.70324675324676,198.10800865800866,170.00194805194806,169.9347269716194,169.57868810484649,166.16736158578263,136.94639249639252,121.43051948051948,120.96055910039254,97.224170274170277,85.001948051948048,85.001948051948048,85.001948051948048,85.001948051948048,85.001948051948048,85.001948051948048,53.126948051948055,51.405450022957972,23.613059163059162,23.613059163059162,23.613059163059162,23.613059163059162,23.613059163059162};

    std::vector<std::shared_ptr<I2::Node>> nodeList;
    std::vector<std::pair<std::shared_ptr<I2::Node>,double>> pageRank;
    int nodeCount = 0;

    EXPECT_NO_THROW(nodeList = std::move(I2::NodeLoader::loadNodesFromFile(DATA_PATH))); // Should load fine without issues
    nodeCount = nodeList.size();

    ASSERT_EQ(nodeCount,expectedPageRank.size());
    pageRank = I2::NodeLoader::computePageRank(nodeList); // Determine the rankings

    ASSERT_EQ(pageRank.size(), nodeCount);
    std::sort(pageRank.begin(),pageRank.end(),I2::NodeLoader::pageRankComparatorGT); // Sort the rankings, descending

    for(int i=0;i<nodeCount;++i)
        EXPECT_DOUBLE_EQ(pageRank[i].second,expectedPageRank[i]); // Ensure each rank matches the expected rank
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}