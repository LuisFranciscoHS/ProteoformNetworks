#include "gtest/gtest.h"
#include <iostream>
#include <list>
#include <bimap_str_int.hpp>

class BimapStrIntFixture : public ::testing::Test {

protected:
    virtual void SetUp() {

        genes.push_back("MAP1ALC3");
        genes.push_back("PARK2");
        genes.push_back("PRKN");
        genes.push_back("PINK1");
        genes.push_back("TOMM40");
        genes.push_back("C19orf1");

        // Create artificial file
        std::string test_name = ::testing::UnitTest::GetInstance()->current_test_info()->name();
        file_name = test_name + "_list_file.csv";
        std::ofstream list_file(file_name);
        for (const auto &gene : genes)
            list_file << gene << "\n";
        list_file.close();
    }

    virtual void TearDown() {
        // Remove artificial file
        int n = file_name.length();
        char file_name_char[n + 1];
        strcpy(file_name_char, file_name.c_str());
        std::remove(file_name_char);
    }

    std::list<std::string> genes;
    std::string file_name;
};

TEST_F(BimapStrIntFixture, CreateBimap) {
    bimap_str_int bimap = createBimap(file_name, true);

    // Check right number of elements
    EXPECT_EQ(bimap.int_to_str.size(), 5);
    EXPECT_EQ(bimap.str_to_int.size(), 5);

    // Check the correct sorted elements
    EXPECT_EQ("C19orf1", bimap.int_to_str[0]);
    EXPECT_EQ("TOMM40", bimap.int_to_str[4]);

    EXPECT_EQ(0, bimap.str_to_int["C19orf1"]);
    EXPECT_EQ(4, bimap.str_to_int["TOMM40"]);
}

TEST_F(BimapStrIntFixture, CreateBimapWithoutHeader) {
    bimap_str_int bimap = createBimap(file_name, false);

    // Check right number of elements
    EXPECT_EQ(bimap.int_to_str.size(), 6);
    EXPECT_EQ(bimap.str_to_int.size(), 6);

    EXPECT_EQ("C19orf1", bimap.int_to_str[0]);
    EXPECT_EQ("TOMM40", bimap.int_to_str[5]);

    EXPECT_EQ(0, bimap.str_to_int["C19orf1"]);
    EXPECT_EQ(5, bimap.str_to_int["TOMM40"]);
}


class CreateBimapFromVectorFixture : public ::testing::Test {
protected:
    virtual void SetUp() {
        vs values = {"3", "2", "1", "1", "2", "3", "4", "4", "5", "0"};
        bimap = createBimap(values);
    }

    bimap_str_int bimap;
};


TEST_F(CreateBimapFromVectorFixture, IntToStrSorted) {
    EXPECT_EQ("0", bimap.int_to_str[0]);
    EXPECT_EQ("1", bimap.int_to_str[1]);
    EXPECT_EQ("2", bimap.int_to_str[2]);
    EXPECT_EQ("3", bimap.int_to_str[3]);
    EXPECT_EQ("4", bimap.int_to_str[4]);
    EXPECT_EQ("5", bimap.int_to_str[5]);
}

TEST_F(CreateBimapFromVectorFixture, CorrectStrToInt) {
    EXPECT_EQ(0, bimap.str_to_int["0"]);
    EXPECT_EQ(1, bimap.str_to_int["1"]);
    EXPECT_EQ(2, bimap.str_to_int["2"]);
    EXPECT_EQ(3, bimap.str_to_int["3"]);
    EXPECT_EQ(4, bimap.str_to_int["4"]);
    EXPECT_EQ(5, bimap.str_to_int["5"]);
}

// This indirectly checks that the duplicate values are gone
TEST_F(CreateBimapFromVectorFixture, CorrectSizes){
    EXPECT_EQ(6, bimap.str_to_int.size());
    EXPECT_EQ(6, bimap.int_to_str.size());
}