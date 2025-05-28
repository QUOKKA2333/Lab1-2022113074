#include "pch.h"
#include "CppUnitTest.h"
#include "WordGraph.h"  

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace WordGraphTests
{
    TEST_CLASS(WordGraphTests)
    {
    private:
        WordGraph graph;

    public:
        TEST_METHOD_INITIALIZE(Setup)
        {
            std::string testText = "The scientist carefully analyzed the data, wrote a detailed report, "
                "and shared the report with the team, but the team requested more data, "
                "so the scientist analyzed it again.";
            graph.buildGraphFromString(testText);
        }

        TEST_METHOD(Test_BridgeWordExists)
        {
            auto result = graph.queryBridgeWords("a", "report");
            Assert::AreEqual(std::string("The bridge word from \"a\" to \"report\" is: detailed"), result);
        }

        TEST_METHOD(Test_NoBridgeWord)
        {
            auto result = graph.queryBridgeWords("scientist", "data");
            Assert::AreEqual(std::string("No bridge words from \"scientist\" to \"data\"!"), result);
        }

        TEST_METHOD(Test_CaseInsensitiveBridgeWord)
        {
            auto result = graph.queryBridgeWords("A", "Report");
            Assert::AreEqual(std::string("The bridge word from \"A\" to \"Report\" is: detailed"), result);
        }

        TEST_METHOD(Test_Word1NotInGraph)
        {
            auto result = graph.queryBridgeWords("unknown", "data");
            Assert::AreEqual(std::string("No \"unknown\" in the graph!"), result);
        }

        TEST_METHOD(Test_Word2NotInGraph)
        {
            auto result = graph.queryBridgeWords("scientist", "unknown");
            Assert::AreEqual(std::string("No \"unknown\" in the graph!"), result);
        }

        TEST_METHOD(Test_EmptyInput)
        {
            auto result = graph.queryBridgeWords("", "");
            Assert::AreEqual(std::string("No \"\" and \"\" in the graph!"), result);
        }
    };
}