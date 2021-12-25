#pragma once
#include <vector>
#include <string>

int binarySearch(std::vector<std::string>& v, int l, int r, std::string target);

void swap(std::string* a, std::string* b);

int partition(std::vector<std::string>& v, int l, int h);

void quickSortIterative(std::vector<std::string>& v, int l, int h);

int linearSearch(std::vector<std::string>& v, std::string target);