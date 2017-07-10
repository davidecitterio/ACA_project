#include <iostream>
#include <list>
#include <fstream>
#include <queue>
#include <map>

#include "rapidxml/rapidxml.hpp"

using namespace rapidxml;
using namespace std;

void loadConfig(map<string, list<string>>& attributes) {
  string type;
  xml_document<> doc;
  xml_node<> * root_node;

  // Read the xml file into a vector
  ifstream theFile ("config.xml");
  if (!theFile.is_open())
  cout << "ERROR: Unable to open config.xml file." << endl;

  vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
  buffer.push_back('\0');
  // Parse the buffer using the xml file parsing library into doc
  doc.parse<0>(&buffer[0]);

  // Find our root node
  root_node = doc.first_node((attributes.find("RootNode")->second.front()).c_str());
  // Find the right setting node
  xml_node<> * node = root_node->first_node((attributes.find("SettingNode")->second.front()).c_str());

  // Iterate over the nodes
  for (; node; node = node->next_sibling())
  {
    type = node->first_attribute("type")->value();
    // Iterate over the attributes
    for (auto attribute_iterator=attributes.begin(); attribute_iterator!=attributes.end(); ++attribute_iterator) {
      if (!type.compare(attribute_iterator->first)){
        cout << attribute_iterator->first << " => " << node->value()<< endl;
        attributes[attribute_iterator->first].push_back(node->value());
        break;
      }
    }
  }

}
