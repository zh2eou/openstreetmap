#include "route_planner.h"
#include <algorithm>
//#include <iostream>
RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    // TODO 2: Use the m_Model.FindClosestNode method to find the closest nodes to the starting and ending coordinates.
    // Store the nodes you find in the RoutePlanner's start_node and end_node attributes.
    start_node = &model.FindClosestNode(start_x, start_y);
    end_node = &model.FindClosestNode(end_x, end_y);
// 	std::cout<< "start node x, y " << start_node->x << ", " <<start_node->y<< " \n";

}


// TODO 3: Implement the CalculateHValue method.
// Tips:
// - You can use the distance to the end_node for the h value.
// - Node objects have a distance method to determine the distance to another node.

float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
    return node->distance(*end_node);
}


// TODO 4: Complete the AddNeighbors method to expand the current node by adding all unvisited neighbors to the open list.
// Tips:
// - Use the FindNeighbors() method of the current_node to populate current_node.neighbors vector with all the neighbors.
// - For each node in current_node.neighbors, set the parent, the h_value, the g_value. 
// - Use CalculateHValue below to implement the h-Value calculation.
// - For each node in current_node.neighbors, add the neighbor to open_list and set the node's visited attribute to true.

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
    current_node->FindNeighbors();
    for (RouteModel::Node *node : current_node->neighbors) {
        node->parent = current_node;
        node->g_value = current_node->g_value + current_node->distance(*node);
        node->h_value = CalculateHValue(node);
        open_list.push_back(node);
        node->visited = true; 
    }
}


// TODO 5: Complete the NextNode method to sort the open list and return the next node.
// Tips:
// - Sort the open_list according to the sum of the h value and g value.
// - Create a pointer to the node in the list with the lowest sum.
// - Remove that node from the open_list.
// - Return the pointer.

bool Compare(const RouteModel::Node* node1, const RouteModel::Node* node2) {
    float node1_sum = node1->h_value + node1->g_value;
    float node2_sum = node2->h_value + node2->g_value;

    return node1_sum > node2_sum;
}

RouteModel::Node *RoutePlanner::NextNode() {
    std::sort(open_list.begin(), open_list.end(), Compare);

    RouteModel::Node *nextNode = open_list[open_list.size() - 1];
//     std::cout << "START: ";
//     for (RouteModel::Node *node : open_list) {
//         std::cout<< "== x, y, sum: " << node->x << ", " <<node->y<< " , "<< node->h_value + node->g_value <<" ==";
//     }
    open_list.pop_back();
//     for (auto node : open_list) {
//         std::cout << "=" << node->h_value + node->g_value << "=";
//     }
//     std::cout << " :END\n";
//     std::cout<< "Next node x, y, sum: " << nextNode->x << ", " <<nextNode->y<< " " << nextNode->h_value + nextNode->g_value << " \n";
//     std::cout << "\n\n";
    return nextNode;
}


// TODO 6: Complete the ConstructFinalPath method to return the final path found from your A* search.
// Tips:
// - This method should take the current (final) node as an argument and iteratively follow the 
//   chain of parents of nodes until the starting node is found.
// - For each node in the chain, add the distance from the node to its parent to the distance variable.
// - The returned vector should be in the correct order: the start node should be the first element
//   of the vector, the end node should be the last element.

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;

    // TODO: Implement your solution here.
//     std::cout << "Start node x, y: " << start_node->x << ", " << start_node->y << "\n";
    while (current_node != start_node) {
      distance += current_node->distance(*current_node->parent);
      path_found.emplace(path_found.begin(), *current_node);
      current_node = current_node->parent;
    }
    path_found.emplace(path_found.begin(), *current_node);
    int cnt = 1;

//     for (RouteModel::Node node : path_found) {
//       std::cout << "node " << cnt << " x, y: " << node.x << ", " << node.y << "\n";
//       cnt++;
//     }
//     std::cout << "End node x, y: " << end_node->x << ", " << end_node->y<< "\n";

    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
    return path_found;

} 


// TODO 7: Write the A* Search algorithm here.
// Tips:
// - Use the AddNeighbors method to add all of the neighbors of the current node to the open_list.
// - Use the NextNode() method to sort the open_list and return the next node.
// - When the search has reached the end_node, use the ConstructFinalPath method to return the final path that was found.
// - Store the final path in the m_Model.path attribute before the method exits. This path will then be displayed on the map tile.

void RoutePlanner::AStarSearch() {
    RouteModel::Node *current_node = nullptr;

    // TODO: Implement your solution here.
    // Set initial node to be start node, set visited to true
    current_node = start_node;
    current_node->visited = true;
    //AddNeighbors(current_node);
    open_list.push_back(start_node);
    // Keep looking for next node until you get to the end node
    while (current_node != end_node && open_list.size() != 0) {
        current_node = NextNode();
//         std :: cout << "current node: g = " << current_node->g_value << " h = " << current_node->h_value << "\n"; 
        AddNeighbors(current_node);

    }
    
    // Construct path
    m_Model.path = ConstructFinalPath(current_node);
}