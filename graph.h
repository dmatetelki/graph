#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <algorithm>

// directed, weighted
/// @todo weight type as param too?

template <typename T>
class Graph {

public:

  typedef size_t size_type;
  typedef T value_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;
  typedef std::ptrdiff_t difference_type;

private:

  struct Edge {

    Edge(const_reference destination, float weight = 0);
    Edge(const Edge& other);
    Edge& operator=(const Edge& other);

    const_pointer m_destination;
    float m_weight;
  };

  struct Vertex {

    Vertex(const_reference data);
    Vertex(const Vertex& other);
    Vertex& operator=(const Vertex& other);
    void addEdge(const_reference destination, float weight = 0);
    void removeEdge(const_reference destination, float weight = 0);
    void removeAllEdgesTo(const_reference destination);

    const_pointer m_data;
    std::vector<Edge> m_edges;
  };


public:

  Graph();

  //  Capacity
  bool empty() const;
  size_type numberOfVertices() const;
  size_type numberOfEdges() const;

  // Modifiers
  bool addVertex(const_reference data);
  bool removeVertex(const_reference data);
  bool addEdge(const_reference source, const_reference destination, float weight = 0);
  bool removeEdge(const_reference source, const_reference destination, float weight = 0);
  bool removeAllEdges(const_reference source, const_reference destination);

  // Lookup
  bool contains(const_reference data) const;
  std::vector<pointer> vertices() const;
  std::vector<pointer> neighboursOf(const_reference data) const;
  std::vector<float> edgesBetween(const_reference source, const_reference destination) const;

private:

  typename std::vector<Vertex >::const_iterator find(const_reference data) const;
  typename std::vector<Vertex >::iterator find(const_reference data);

  std::vector<Vertex> m_vertices;
};


// Edge

template <typename T>
Graph<T>::Edge::Edge(const_reference destination, float weight)
  : m_destination(&destination)
  , m_weight(weight)
{

}

template <typename T>
Graph<T>::Edge::Edge(const Edge& other)
  : m_destination(other.m_destination)
  , m_weight(other.m_weight)
{

}

template <typename T>
typename Graph<T>::Edge& Graph<T>::Edge::operator=(const Edge& other)
{
  if (this != &other) {
    m_destination = other.m_destination;
    m_weight = other.m_weight;
  }

  return *this;
}


// Vertex

template <typename T>
Graph<T>::Vertex::Vertex(const_reference data)
  : m_data(&data)
  , m_edges()
{

}

template <typename T>
Graph<T>::Vertex::Vertex(const Vertex& other)
  : m_data(other.m_data)
  , m_edges(other.m_edges)
{

}

template <typename T>
typename Graph<T>::Vertex& Graph<T>::Vertex::operator=(const Vertex& other)
{
  if (this != &other) {
    m_data = other.m_data;
    m_edges.clear();
    m_edges = other.m_edges;
  }

  return *this;
}

template <typename T>
void Graph<T>::Vertex::addEdge(const_reference destination, float weight)
{
  Edge e(destination, weight);
  m_edges.push_back(e);
}

template <typename T>
void Graph<T>::Vertex::removeEdge(const_reference destination, float weight)
{
  m_edges.erase(std::find_if(m_edges.begin(), m_edges.end(),
                             [&destination, &weight](const Edge& e)
                             { return e.m_destination == destination &&
                                      e.m_weight == weight;}));
}

template <typename T>
void Graph<T>::Vertex::removeAllEdgesTo(const_reference destination)
{
  std::remove_if(m_edges.begin(), m_edges.end(),
                 [&destination](const Edge& e)
                 { return e.m_destination == destination; });
}


// Graph

template <typename T>
Graph<T>::Graph()
  : m_vertices()
{

}

template <typename T>
bool Graph<T>::empty() const
{
  return m_vertices.empty();
}

template <typename T>
typename Graph<T>::size_type Graph<T>::numberOfVertices() const
{
  return m_vertices.size();
}

template <typename T>
typename Graph<T>::size_type Graph<T>::numberOfEdges() const
{
  return std::accumulate(m_vertices.begin(), m_vertices.end(), 0,
                  [](int sum, const Vertex& v)
                  { return sum + v.m_edges.size(); });
}

template <typename T>
bool Graph<T>::addVertex(const_reference data)
{
  if (find(data) != m_vertices.end())
    return false;

  Vertex v(data);
  m_vertices.push_back(v);
  return true;
}

template <typename T>
bool Graph<T>::removeVertex(const_reference data)
{
  typename std::vector<Vertex>::iterator it = find(data);
  if (it == m_vertices.end())
    return false;

  m_vertices.erase(it);
  return true;
}

template <typename T>
bool Graph<T>::addEdge(const_reference source, const_reference destination, float weight)
{
  typename std::vector<Vertex>::iterator source_it = find(source);
  if (source_it == m_vertices.end())
    return false;

  typename std::vector<Vertex>::iterator destination_it = find(destination);
  if (destination_it == m_vertices.end())
    return false;

  (*source_it).addEdge(destination, weight);
  return true;
}

template <typename T>
bool Graph<T>::removeEdge(const_reference source, const_reference destination, float weight)
{
  typename std::vector<Vertex>::iterator it = find(source);
  if (it == m_vertices.end())
    return false;

  (*it).removeEdge(destination, weight);
  return true;
}

template <typename T>
bool Graph<T>::removeAllEdges(const_reference source, const_reference destination)
{
  typename std::vector<Vertex>::iterator it = find(source);
  if (it == m_vertices.end())
    return false;

  (*it).removeAllEdgesEdge(destination);
  return true;
}

template <typename T>
bool Graph<T>::contains(const_reference data) const
{
  return find(data) != m_vertices.end();
}

template <typename T>
std::vector<typename Graph<T>::pointer> Graph<T>::vertices() const
{
  std::vector<T*> retval;
  std::for_each(m_vertices.begin(), m_vertices.end(),
                [&retval](const Vertex& v)
                { retval.push_back( const_cast<T*>(v.m_data)); });
  return retval;
}

template <typename T>
std::vector<typename Graph<T>::pointer> Graph<T>::neighboursOf(const_reference data) const
{
  typename std::vector<T*> retval;
  typename std::vector<Vertex >::const_iterator vertex_it = find(data);
  if (vertex_it == m_vertices.end())
    return retval;

  std::for_each((*vertex_it).m_edges.begin(), (*vertex_it).m_edges.end(),
                [&retval](const Edge& e)
                { retval.push_back( const_cast<T*>(e.m_destination)); });

  return retval;
}

template <typename T>
std::vector<float> Graph<T>::edgesBetween(const_reference source, const_reference destination) const
{
  std::vector<float> retval;
  typename std::vector<Vertex>::const_iterator vertex_it = find(source);
  if (vertex_it == m_vertices.end())
    return retval;

  std::for_each((*vertex_it).m_edges.begin(), (*vertex_it).m_edges.end(),
                [&retval, &destination](const Edge& e)
                { if (*(e.m_destination) == destination)
                    retval.push_back(e.m_weight); });

  return retval;
}


template <typename T>
typename std::vector<typename Graph<T>::Vertex >::const_iterator Graph<T>::find(const_reference data) const
{
  return std::find_if(m_vertices.begin(), m_vertices.end(),
                      [&data](const Vertex& v)
                      { return *(v.m_data) == data; });
}

template <typename T>
typename std::vector<typename Graph<T>::Vertex >::iterator Graph<T>::find(const_reference data)
{
  return std::find_if(m_vertices.begin(), m_vertices.end(),
                      [&data](const Vertex& v)
                      { return *(v.m_data) == data; });
}


#endif // GRAPH_H
