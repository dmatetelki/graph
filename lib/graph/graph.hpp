#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <list>
#include <set>

#include <algorithm>


template <typename V,
          typename E = int>
class Graph {

public:

  typedef size_t size_type;
  typedef V value_type;
  typedef V* pointer;
  typedef const V* const_pointer;
  typedef V& reference;
  typedef const V& const_reference;
  typedef std::ptrdiff_t difference_type;

  typedef E weight_type;
  typedef const E& const_weight_reference;

  class Edge;
  class vertex_iterator;
  class edge_iterator;

private:

  struct Vertex;
  typedef std::vector<Vertex> v_container;
  typedef typename v_container::iterator v_iterator;
  typedef typename v_container::const_iterator v_const_iterator;

  struct EdgeTo {

    EdgeTo() = delete;
    EdgeTo(v_const_iterator destination, const_weight_reference weight = weight_type());
    EdgeTo(const EdgeTo& o) : m_destination(o.m_destination), m_weight(o.m_weight) {}
    EdgeTo& operator=(EdgeTo o) { swap(o); return *this; }
    void swap(EdgeTo& o);
    bool operator==(const EdgeTo& o) const;

    v_const_iterator m_destination;
    weight_type m_weight;
  };

  struct Vertex {

    // because of parallell accumulate, ctor(int) is needed and that could conflict
    // Vertex(const_reference data) : m_data(data), m_edges() {}
    Vertex() : m_data(), m_edges()  {}

    Vertex(const Vertex& o) : m_data(o.m_data), m_edges(o.m_edges) {}
    Vertex& operator=(Vertex o) { swap(o); return *this; }
    void swap(Vertex& o) { std::swap(m_data, o.m_data); std::swap(m_edges, o.m_edges);}
    bool operator==(const Vertex& o) const;

    // parallell accumulate requires both
    Vertex(int) : Vertex()  {}
    explicit operator int() const { return (int)m_edges.size(); }

    void addEdge(v_iterator destination, const_weight_reference weight = weight_type());
    void removeEdge(v_iterator destination, const_weight_reference weight = weight_type());
    void removeAllEdgesTo(v_iterator destination);
    std::vector<Edge> edges() const;

    value_type m_data;
    std::list<EdgeTo> m_edges;
  };

public:

  struct Edge {

  public:

    Edge() : source(), destination(), weight() {}
    Edge(const_reference source, const_reference destination, const_weight_reference weight);
    Edge(const Edge& o);
    Edge& operator=(Edge o) { swap(o); return *this; }
    void swap(Edge& o);
    bool operator==(const Edge& o) const { return source == o.source && destination == o.destination && weight == o.weight; }

    value_type source;
    value_type destination;
    weight_type weight;
  };

  typedef Edge* edge_pointer;
  typedef Edge& edge_reference;

// public:

  Graph(bool isdirected = true) : m_directed(isdirected), m_vertices() {}
  Graph(const Graph<V, E>& o) : m_directed(o.m_directed), m_vertices(o.m_vertices) {}
  Graph(std::initializer_list<V> vertex_list);
  Graph(std::initializer_list<Edge> edge_list);

  Graph<V, E>& operator=(Graph<V, E> o) { swap(o); return *this; }
  void swap(Graph& o) { std::swap (m_directed, o.m_directed); std::swap(m_vertices, o.m_vertices); }

  // Properties
  bool directed() const { return m_directed; }

  //  Capacity
  bool empty() const  { return m_vertices.empty(); }
  size_type numberOfVertices() const { return m_vertices.size(); }
  size_type numberOfEdges() const;


  /// @todo rename Vertex & Edge?
  // Modifiers
  void addVertex(const_reference data);
  void removeVertex(const_reference data);
  void addEdge(const_reference source, const_reference destination, const_weight_reference weight = weight_type());
  void removeEdge(const_reference source, const_reference destination, const_weight_reference weight = weight_type());
  void removeEdges(const_reference source, const_reference destination);

  // Lookup
  bool contains(const_reference data) const { return find(data) != m_vertices.end(); }
  std::vector<value_type> vertices() const;
  std::vector<value_type> neighboursOf(const_reference data) const;
  std::vector<weight_type> weights(const_reference source, const_reference destination) const;
  std::vector<Edge> edges() const;

  // iterators

  /// @todo make it random access
  class vertex_iterator : public std::iterator<std::forward_iterator_tag,
                                               value_type,
                                               difference_type,
                                               pointer,
                                               reference>
  {
  friend class Graph;

  public:
    typedef vertex_iterator self_type;
    typedef vertex_iterator& reference_self_type;
    typedef const vertex_iterator& const_reference_self_type;

    vertex_iterator() : m_it() {}
    ~vertex_iterator() {}
    vertex_iterator(const_reference_self_type o) : m_it(o.m_it) {}
    reference_self_type operator=(self_type o) { swap(o); return *this; }
    void swap(reference_self_type o) { std::swap(m_it, o.m_it); }

    const_reference operator*() { return m_it->m_data; }
    const_pointer operator->() { return &m_it->m_data; }
    self_type &operator++() { ++m_it; return *this; }
    self_type operator++(int) { self_type tmp(*this); ++(*this); return tmp; }
    self_type operator+(difference_type n) { self_type tmp(*this); tmp.pos_ += n; return tmp; }
    self_type &operator+=(difference_type n) {  m_it += n; return *this; }
    bool operator==(const_reference_self_type o) const { return m_it == o.m_it; }
    bool operator!=(const_reference_self_type o) const { return !(*this == o); }

  private:
    vertex_iterator(v_iterator it) : m_it(it) {}
    vertex_iterator(v_const_iterator it) : m_it(it) {}

    v_const_iterator m_it;
  };

  vertex_iterator vertex_begin() { return vertex_iterator(m_vertices.begin()); }
  const vertex_iterator vertex_begin() const { return vertex_iterator(m_vertices.begin()); }
  vertex_iterator vertex_end() { return vertex_iterator(m_vertices.end()); }
  const vertex_iterator vertex_end() const { return vertex_iterator(m_vertices.end()); }

  class edge_iterator : public std::iterator<std::forward_iterator_tag,
                                             Edge,
                                             difference_type,
                                             edge_pointer,
                                             edge_reference>
  {
  friend class Graph;

  public:
    typedef edge_iterator self_type;
    typedef edge_iterator& reference_self_type;
    typedef const edge_iterator& const_reference_self_type;

    edge_iterator() : m_vertices(),  m_vertex_it(), m_edge_it(), m_edge() {}
    ~edge_iterator() {}
    edge_iterator(const_reference_self_type o);
    reference_self_type operator=(self_type o) { swap(o); return *this; }
    void swap(reference_self_type other);

    edge_reference operator*() { resetEdge(); return m_edge; }
    edge_pointer operator->() { resetEdge(); return &m_edge; }
    self_type &operator++() { advance(1); return *this; }
    self_type operator++(int) { self_type tmp(*this); advance(1); return tmp; }
    self_type operator+(difference_type n) { self_type tmp(*this); tmp.pos_ += n; return tmp; }
    self_type &operator+=(difference_type n) {  advance(n); return *this; }
    bool operator==(const_reference_self_type o) const;
    bool operator!=(const_reference_self_type o) const { return !(*this == o); }

  private:
    edge_iterator(v_container vertices, bool begin = true);
    void resetEdge();
    void advance(int n);

    v_container m_vertices;
    v_iterator m_vertex_it;
    typename std::list<EdgeTo>::iterator m_edge_it;
    Edge m_edge;
  };

  edge_iterator edge_begin() { return edge_iterator(m_vertices); }
  const edge_iterator edge_begin() const { return edge_iterator(m_vertices); }
  edge_iterator edge_end() { return edge_iterator(m_vertices, false); }
  const edge_iterator edge_end() const { return edge_iterator(m_vertices, false); }

private:
  v_const_iterator find(const_reference data) const;
  v_iterator find(const_reference data);
  v_const_iterator findAndCheck(const_reference data, bool existence_expected = true) const;
  v_iterator findAndCheck(const_reference data, bool existence_expected = true);
  void adjustEdges(v_iterator vit);

  const bool m_directed;
  v_container m_vertices;
};


// Edge

template <typename V, typename E>
inline Graph<V, E>::Edge::Edge(const_reference s, const_reference d, const_weight_reference w)
  : source(s)
  , destination(d)
  , weight(w)
{}

template <typename V, typename E>
inline Graph<V, E>::Edge::Edge(const Edge& o)
  : source(o.source)
  , destination(o.destination)
  , weight(o.weight)
{}

template <typename V, typename E>
inline void Graph<V, E>::Edge::swap(Edge& o)
{
  std::swap(source, o.source);
  std::swap(destination, o.destination);
  std::swap(weight, o.weight);
}


// edge iterator

template <typename V, typename E>
inline Graph<V, E>::edge_iterator::edge_iterator(const_reference_self_type o)
  : m_vertices(o.m_vertices)
  , m_vertex_it(o.m_vertex_it)
  , m_edge_it(o.m_edge_it)
  , m_edge()
{}

template <typename V, typename E>
bool Graph<V, E>::edge_iterator::operator==(const_reference_self_type o) const
{
  const bool this_is_at_end = m_vertex_it == m_vertices.end();
  const bool other_is_at_end = o.m_vertex_it == o.m_vertices.end();
  if ( this_is_at_end && other_is_at_end ) return true;
  if ( this_is_at_end != other_is_at_end ) return false;

  return *m_vertex_it == *(o.m_vertex_it) && *m_edge_it == *(o.m_edge_it);
}

template <typename V, typename E>
inline void Graph<V, E>::edge_iterator::swap(reference_self_type other)
{
  std::swap(m_vertices, other.m_vertices);
  std::swap(m_vertex_it, other.m_vertex_it);
  std::swap(m_edge_it, other.m_edge_it);
  std::swap(m_edge, other.m_edge);
}

template <typename V, typename E>
inline Graph<V, E>::edge_iterator::edge_iterator(v_container vertices, bool begin)
  : m_vertices(vertices), m_vertex_it(), m_edge_it(), m_edge()
{
  if (begin) {
    m_vertex_it = m_vertices.begin();
    while (m_vertex_it != m_vertices.end() && m_vertices.empty())
      ++m_vertex_it;

    if (m_vertex_it != m_vertices.end())
      m_edge_it = m_vertex_it->m_edges.begin();
  } else {
    m_vertex_it = m_vertices.end();
  }
}

template <typename V, typename E>
inline void Graph<V, E>::edge_iterator::resetEdge()
{
  if (m_vertex_it == m_vertices.end() || (*m_vertex_it).m_edges.empty()) {
    m_edge = Edge();
  } else {
    m_edge = Edge( m_vertex_it->m_data, (m_edge_it->m_destination)->m_data, m_edge_it->m_weight);
  }
}

template <typename V, typename E>
void Graph<V, E>::edge_iterator::advance(int n)
{
  while (n > 0 && m_vertex_it != m_vertices.end()) {
    const int edgesAhead = std::distance(m_edge_it, m_vertex_it->m_edges.end()) - 1;
    if (n <= edgesAhead) {
      std::advance(m_edge_it, n);
      return;
    }

    if (edgesAhead > 0) n -= edgesAhead;
    ++m_vertex_it;

    if (m_vertex_it != m_vertices.end()) {
      m_edge_it = m_vertex_it->m_edges.begin();
      if (m_edge_it != m_vertex_it->m_edges.end())
        --n;
    }
  }
}

// EdgeTo
template <typename V, typename E>
inline Graph<V, E>::EdgeTo::EdgeTo(v_const_iterator destination, const_weight_reference weight)
  : m_destination(destination)
  , m_weight(weight)
{}

template <typename V, typename E>
inline void Graph<V, E>::EdgeTo::swap(EdgeTo& o)
{
  std::swap(m_destination, o.m_destination);
  std::swap(m_weight, o.m_weight);
}

template <typename V, typename E>
inline bool Graph<V, E>::EdgeTo::operator==(const EdgeTo& other) const
{
  return m_destination == other.m_destination &&
         m_weight == other.m_weight;
}


// Vertex

template <typename V, typename E>
inline bool Graph<V, E>::Vertex::operator==(const Vertex& other) const
{
  return m_data == other.m_data &&
         m_edges.size() == other.m_edges.size() &&
         m_edges == other.m_edges;
}

template <typename V, typename E>
inline void Graph<V, E>::Vertex::addEdge(v_iterator destination, const_weight_reference weight)
{
  m_edges.push_back(EdgeTo(destination, weight));
}

template <typename V, typename E>
inline void Graph<V, E>::Vertex::removeEdge(v_iterator destination, const_weight_reference weight)
{
  m_edges.erase(
    std::find_if(m_edges.begin(), m_edges.end(),
                 [&destination, &weight](const EdgeTo& e)
                 { return e.m_destination == destination && e.m_weight == weight;}));
}

template <typename V, typename E>
inline void Graph<V, E>::Vertex::removeAllEdgesTo(v_iterator destination)
{
  m_edges.erase(
    std::remove_if(m_edges.begin(), m_edges.end(),
                  [&destination](const EdgeTo& e)
                  { return e.m_destination == destination; }),
    m_edges.end());
}

template <typename V, typename E>
inline std::vector<typename Graph<V, E>::Edge> Graph<V, E>::Vertex::edges() const
{
  std::vector<Graph<V, E>::Edge> retval;
  for (const EdgeTo& e : m_edges)
    retval.push_back(Edge(m_data, (e.m_destination)->m_data, e.m_weight));

  return retval;
}


// Graph

template <typename V, typename E>
Graph<V, E>::Graph(std::initializer_list<V> vertex_list)
  : Graph<V, E>()
{
  for(const V& v : vertex_list)
    addVertex(v);
}

template <typename V, typename E>
Graph<V, E>::Graph(std::initializer_list<Edge> edge_list)
  : Graph<V, E>()
{
  for (const Edge& e : edge_list )
    addEdge(e.source, e.destination, e.weight);
}

template <typename V, typename E>
inline typename Graph<V, E>::size_type Graph<V, E>::numberOfEdges() const
{
  return std::accumulate(m_vertices.begin(), m_vertices.end(), 0,
                         [](int sum, const Vertex& v)
                         { return sum + v.m_edges.size(); });
}

template <typename V, typename E>
inline void Graph<V, E>::addVertex(const_reference data)
{
  if (find(data) != m_vertices.end())
    return;

  Vertex v;
  v.m_data = data;
  m_vertices.push_back(v);
}

template <typename V, typename E>
inline void Graph<V, E>::removeVertex(const_reference data)
{
  v_iterator it = find(data);
  if (it == m_vertices.end())
    return;

  std::for_each(m_vertices.begin(), m_vertices.end(),
                [&it] (Vertex& v)
                { v.removeAllEdgesTo(it); } );

  m_vertices.erase(it);
  adjustEdges(it);
}

template <typename V, typename E>
void Graph<V, E>::addEdge(const_reference source, const_reference destination, const_weight_reference weight)
{
  addVertex(source);
  addVertex(destination);

  v_iterator source_it = find(source);
  v_iterator destination_it = find(destination);

  source_it->addEdge(destination_it, weight);
  if (!m_directed && source != destination)
    destination_it->addEdge(source_it, weight);
}

template <typename V, typename E>
inline void Graph<V, E>::removeEdge(const_reference source, const_reference destination, const_weight_reference weight)
{
  v_iterator source_it = find(source);
  if (source_it == m_vertices.end())
    return;

  v_iterator destination_it = find(destination);
  if (destination_it == m_vertices.end())
    return;

  source_it->removeEdge(destination, weight);
  if (!m_directed)
    destination_it->removeEdge(source, weight);
}

template <typename V, typename E>
inline void Graph<V, E>::removeEdges(const_reference source, const_reference destination)
{
  v_iterator source_it = find(source);
  if (source_it == m_vertices.end())
    return;

  v_iterator destination_it = find(destination);
  if (destination_it == m_vertices.end())
    return;

  source_it->removeAllEdgesTo(destination_it);
  if (!m_directed)
    destination_it->removeAllEdgesTo(source_it);
}

template <typename V, typename E>
inline std::vector<typename Graph<V, E>::value_type> Graph<V, E>::vertices() const
{
  std::vector<value_type> retval;
  for (const Vertex& v : m_vertices)
    retval.push_back(v.m_data);

  return retval;
}

template <typename V, typename E>
std::vector<typename Graph<V, E>::value_type> Graph<V, E>::neighboursOf(const_reference data) const
{
  typename std::vector<value_type> retval;
  v_const_iterator vertex_it = find(data);
  if (vertex_it == m_vertices.end() || vertex_it->m_edges.empty())
    return retval;

  std::set<v_const_iterator> tmp;
  for (const EdgeTo& e : vertex_it->m_edges)
    if (tmp.insert(e.m_destination).second)
      retval.push_back((e.m_destination)->m_data);

  return retval;
}

template <typename V, typename E>
std::vector<E> Graph<V, E>::weights(const_reference source, const_reference destination) const
{
  std::vector<E> retval;
  v_const_iterator vertex_it = find(source);
  if (vertex_it == m_vertices.end())
    return retval;

  if (find(destination) == m_vertices.end())
    return retval;

  for (const EdgeTo& e : vertex_it->m_edges)
    if (e.m_destination->m_data == destination)
      retval.push_back(e.m_weight);

  return retval;
}

template <typename V, typename E>
inline std::vector<typename Graph<V, E>::Edge> Graph<V, E>::edges() const
{
  std::vector<typename Graph<V, E>::Edge> retval;
  for (const Vertex& v : m_vertices) {
    const std::vector<Edge> e = v.edges();
    retval.insert(retval.end(), e.begin(), e.end());
  }

  return retval;
}


template <typename V, typename E>
inline typename Graph<V, E>::v_const_iterator
Graph<V, E>::find(const_reference data) const
{
  return std::find_if(m_vertices.begin(), m_vertices.end(),
                      [&data](const Vertex& v)
                      { return v.m_data == data; });
}

template <typename V, typename E>
inline typename Graph<V, E>::v_iterator
Graph<V, E>::find(const_reference data)
{
  return std::find_if(m_vertices.begin(), m_vertices.end(),
                      [&data](const Vertex& v)
                      { return v.m_data == data; });
}

template <typename V, typename E>
void Graph<V, E>::adjustEdges(v_iterator deleted_vit)
{
  std::for_each(m_vertices.begin(), m_vertices.end(),
                [&deleted_vit](Vertex& v)
                { for (EdgeTo& e : v.m_edges)
                    if (e.m_destination > deleted_vit)
                      --e.m_destination;
                });
}


#endif // GRAPH_H
