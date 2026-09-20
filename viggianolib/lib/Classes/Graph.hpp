#pragma once
namespace mpv{
	template<typename> class Neighbors;
	template<typename> class NeighborValues;
	template<typename> class ConstNeighborValues;
	template<typename> class Edges;
	template<typename> class ConstEdges;

	template<typename T> struct EdgeBase{
		T data;
		template<typename... Args>
		EdgeBase(Args&&... args):data(static_cast<Args&&>(args)...){}
	};
	template<> struct EdgeBase<void>{};

	template<typename T> 
	class Edge:public EdgeBase<T>{
		using node_handle_iterator=typename List<typename List<Edge<T>>::iterator>::iterator;// vh apunta hacia el handle de la arista que une uv en el vertice v. hy lo mismo pero con el vertice u
		node_handle_iterator vh=nullptr,uh=nullptr;
		node_handle_iterator follow_and_get_handle_from(size_t vertex)const noexcept{
			return vertex==v?uh:vh;
		}
	public:
		const size_t v,u;
		size_t follow_from(size_t vertex)const noexcept{
			return vertex==v?u:v;
		}
		template<typename... Args>
		Edge(size_t v,size_t u,Args&&... args):EdgeBase<T>(static_cast<Args&&>(args)...),v(v),u(u){}
		Edge(const Edge&)=default;
		Edge& operator=(const Edge&)=delete;
		template<typename Out,typename E>friend Out& operator<<(Out&,const Edge<E>&);
		template<typename,typename> friend class Graph;
	};
	template<typename T,typename U>
	struct Vertex{
		Vertex(const Vertex&)=delete;
		Vertex& operator=(const Vertex&)=delete;
		Vertex(Vertex&&)=default;
		Vertex& operator=(Vertex&&)=default;
		T data;
		List<U> edges;
		template<typename... Args>
		Vertex(Args&&... args):data(static_cast<Args&&>(args)...){}
	};
	template<typename U>
	struct Vertex<void,U>{
		Vertex(const Vertex&)=delete;
		Vertex& operator=(const Vertex&)=delete;
		Vertex(Vertex&&)=default;
		Vertex& operator=(Vertex&&)=default;
		List<U> edges;
		Vertex()=default;
	};
	template<typename V,typename E>
	class Graph{
	public:
		using edge_handle=typename List<Edge<E>>::iterator;
		using const_edge_handle=typename List<Edge<E>>::const_iterator;
		using vertex_type=Vertex<V,edge_handle>;
		using edge_type=Edge<E>;
		using vertex_value=V;
		using edge_value=E;

		class const_iterator;
		class iterator{
		public:
			using iterator_category=bidirectional_iterator_tag;
			using pointer=vertex_value*;
			using value_type=vertex_value;
			using reference=size_t;
			using difference_type=ptrdiff_t;
			using Ptr=typename HoleVector<vertex_type>::iterator;
		private:
			HoleVector<vertex_type>& vertexes;
			Ptr it;
		public:
			constexpr iterator(HoleVector<vertex_type>& vertexes,Ptr iter)noexcept:vertexes(vertexes),it(iter){}
			constexpr reference operator*()const noexcept{
				return it.get_raw_index();
			}
			template<typename T=vertex_value,enable_if_t<!is_void_v<T>>* = nullptr>
			constexpr pointer operator->()const noexcept{
				return addressof(it->data);
			}
			template<typename T=vertex_value,enable_if_t<!is_void_v<T>>* = nullptr>
			constexpr T& get()const noexcept{
				return it->data;
			}
			constexpr iterator& operator++()noexcept{
				++it;
				return *this;
			}
			constexpr iterator operator++(int)noexcept{
				iterator aux=*this;
				++it;
				return aux;
			}
			constexpr iterator& operator--()noexcept{
				--it;
				return *this;
			}
			constexpr iterator operator--(int)noexcept{
				iterator aux=*this;
				--it;
				return aux;
			}
			constexpr bool operator==(const iterator& other)const noexcept{
				return this->it==other.it;
			}
			constexpr bool operator!=(const iterator& other)const noexcept{
				return this->it!=other.it;
			}
			constexpr bool operator>=(const iterator& other)const noexcept{
				return this->it>=other.it;
			}
			constexpr bool operator<=(const iterator& other)const noexcept{
				return this->it<=other.it;
			}
			constexpr bool operator>(const iterator& other)const noexcept{
				return this->it>other.it;
			}
			constexpr bool operator<(const iterator& other)const noexcept{
				return this->it<other.it;
			}
			friend class const_iterator;
		};
		class const_iterator{
		public:
			using iterator_category=bidirectional_iterator_tag;
			using pointer=const vertex_value*;
			using value_type=const vertex_value;
			using reference=size_t;
			using difference_type=ptrdiff_t;
			using Ptr=typename HoleVector<vertex_type>::const_iterator;
		private:
			const HoleVector<vertex_type>& vertexes;
			Ptr it;
		public:
			constexpr const_iterator(const iterator& nonconst_it)noexcept:vertexes(nonconst_it.vertexes),it(nonconst_it.it){}
			constexpr const_iterator(const HoleVector<vertex_type>& vertexes,Ptr iter)noexcept:vertexes(vertexes),it(iter){}
			constexpr reference operator*()const noexcept{
				return it.get_raw_index();
			}
			template<typename T=vertex_value,enable_if_t<!is_void_v<T>>* = nullptr>
			constexpr pointer operator->()const noexcept{
				return addressof(it->data);
			}
			template<typename T=vertex_value,enable_if_t<!is_void_v<T>>* = nullptr>
			constexpr const T& get()const noexcept{
				return it->data;
			}
			constexpr const_iterator& operator++()noexcept{
				++it;
				return *this;
			}
			constexpr const_iterator operator++(int)noexcept{
				const_iterator aux=*this;
				++it;
				return aux;
			}
			constexpr const_iterator& operator--()noexcept{
				--it;
				return *this;
			}
			constexpr const_iterator operator--(int)noexcept{
				const_iterator aux=*this;
				--it;
				return aux;
			}
			constexpr bool operator==(const const_iterator& other)const noexcept{
				return this->it==other.it;
			}
			constexpr bool operator!=(const const_iterator& other)const noexcept{
				return this->it!=other.it;
			}
			constexpr bool operator>=(const const_iterator& other)const noexcept{
				return this->it>=other.it;
			}
			constexpr bool operator<=(const const_iterator& other)const noexcept{
				return this->it<=other.it;
			}
			constexpr bool operator>(const const_iterator& other)const noexcept{
				return this->it>other.it;
			}
			constexpr bool operator<(const const_iterator& other)const noexcept{
				return this->it<other.it;
			}
		};
		constexpr size_t size()const noexcept{
			return vertexes.size();
		}
		constexpr const_iterator begin()const noexcept{
			return const_iterator(vertexes,vertexes.begin());
		}
		constexpr const_iterator end()const noexcept{
			return const_iterator(vertexes,vertexes.end());
		}
		constexpr iterator begin()noexcept{
			return iterator(vertexes,vertexes.begin());
		}
		constexpr iterator end()noexcept{
			return iterator(vertexes,vertexes.end());
		}
	private:
		HoleVector<vertex_type> vertexes;
		List<Edge<E>> edges_list;
	public:
		Graph()=default;
		Graph(const Graph& other){
			vertexes.copy_dist(other.vertexes);
			for(const size_t v:other){
				if constexpr(is_void_v<vertex_value>) emplace_vertex();
				else emplace_vertex(other[v]);
			}
			for(const Edge<E>& edge:other.edge_list()){
				if constexpr(is_void_v<edge_value>) emplace_edge(edge.v,edge.u);
				else emplace_edge(edge.v,edge.u,edge.data);
			}
		}
		Graph(Graph&&)=default;
		Graph& operator=(const Graph&)=delete;
		Graph& operator=(Graph&&)=default;
		template<typename T=V,enable_if_t<!is_void_v<T>>* = nullptr>
		T& operator[](size_t v){
			return vertexes[v].data;
		}
		template<typename T=V,enable_if_t<!is_void_v<T>>* = nullptr>
		const T& operator[](size_t v)const{
			return vertexes[v].data;
		}
		bool vertex_exist(size_t v)const noexcept{
			return v<vertexes.get_internal_vector().size() && vertexes.has_value_at(v);
		}
		const List<Edge<E>>& get_edges()const noexcept{
			return edges_list;
		}
		template<typename... Args>
		size_t emplace_vertex(Args&&... args){
			return vertexes.emplace(static_cast<Args&&>(args)...).get_raw_index();
		}
		template<typename T=V,enable_if_t<is_default_constructible_v<T> || is_void_v<T>>* = nullptr>
		size_t push_vertex(){
			return emplace_vertex();
		}
		template<typename T=V,enable_if_t<is_convertible_v<T,V>>* = nullptr>
		size_t push_vertex(T&& val){
			return emplace_vertex(static_cast<T&&>(val));
		}
		template<typename... Args>
		edge_handle emplace_edge(size_t v,size_t u,Args&&... args){
			if(!(vertex_exist(v) && vertex_exist(u))) return edges_list.end();
			edge_handle eh=edges_list.emplace(edges_list.end(),v,u,static_cast<Args&&>(args)...);
			eh->vh = vertexes[v].edges.insert(vertexes[v].edges.end(),eh);
			eh->uh = vertexes[u].edges.insert(vertexes[u].edges.end(),eh);
			return eh;
		}
		template<typename T=E,enable_if_t<is_default_constructible_v<T> || is_void_v<T>>* = nullptr>
		edge_handle insert_edge(size_t v,size_t u){
			return emplace_edge(v,u);
		}
		template<typename T=E,enable_if_t<is_convertible_v<T,E>>* = nullptr>
		edge_handle insert_edge(size_t v,size_t u,T&& val){
			return emplace_edge(v,u,static_cast<T&&>(val));
		}
		bool remove_edge_at(size_t v,size_t u){
			for(edge_handle eh:vertexes[v].edges){
				if(eh->follow_from(v)==u) {
					remove_edge(eh);
					return true;
				}
			}
			return false;
		}
		void remove_edge(const_edge_handle eh){
			vertexes[eh->v].edges.del(eh->vh);
			vertexes[eh->u].edges.del(eh->uh);
			edges_list.del(eh);
		}
		bool remove_vertex(size_t v){
			if(!vertex_exist(v)) return false;
			while(vertexes[v].edges.size()>0)
				remove_edge(*vertexes[v].edges.begin());
			vertexes.del_at(v);
			return true;
		}
		Neighbors<Graph> neighbors(size_t v)const{
			return Neighbors<Graph>(*this,v);
		}
		template<typename T=V,enable_if_t<!is_void_v<T>>* = nullptr>
		NeighborValues<Graph> neighbor_values(size_t v){
			return NeighborValues<Graph>(*this,v);
		}
		template<typename T=V,enable_if_t<!is_void_v<T>>* = nullptr>
		ConstNeighborValues<Graph> neighbor_values(size_t v)const{
			return ConstNeighborValues<Graph>(*this,v);
		}
		Edges<Graph> edges(size_t v){
			return Edges<Graph>(*this,v);
		}
		ConstEdges<Graph> edges(size_t v)const{
			return ConstEdges<Graph>(*this,v);
		}
		Viewer<List<Edge<E>>> edge_list(){
			return edges_list;
		}
		ConstViewer<List<Edge<E>>> edge_list()const{
			return edges_list;
		}
		template<typename> friend class Neighbors;
		template<typename> friend class NeighborValues;
		template<typename> friend class ConstNeighborValues;
		template<typename> friend class Edges;
		template<typename> friend class ConstEdges;
		template<typename Out,typename Vt,typename Et> friend Out& operator<<(Out&,const Graph<Vt,Et>&);
	};

	template<typename Graph_t>
	class Edges{
		Graph_t& graph;
		size_t vertex;
	public:
		class const_iterator;
		class iterator{
			public:
				using Ptr=typename List<typename Graph_t::edge_handle>::const_iterator;
				using iterator_category=bidirectional_iterator_tag;
				using value_type=typename Graph_t::edge_type;
				using pointer=typename Graph_t::edge_handle;
				using difference_type=ptrdiff_t;
				using reference=value_type&;
			private:
				Ptr ptr;
				size_t vertex;
			public:
				iterator(Ptr ptr,size_t vertex)noexcept:ptr(ptr),vertex(vertex){}
				constexpr reference operator*()const{
					return **ptr;
				}
				constexpr pointer operator->()const{
					return *ptr;
				}
				constexpr size_t follow()const{
					return (**ptr).follow_from(vertex);
				}
				constexpr iterator& operator++()noexcept{
					++ptr;
					return *this;
				}
				constexpr iterator operator++(int)noexcept{
					iterator aux=*this;
					++ptr;
					return aux;
				}
				constexpr iterator& operator--()noexcept{
					--ptr;
					return *this;
				}
				constexpr iterator operator--(int)noexcept{
					iterator aux=*this;
					--ptr;
					return aux;
				}
				constexpr bool operator==(const iterator& other)const noexcept{
					return this->ptr==other.ptr;
				}
				constexpr bool operator!=(const iterator& other)const noexcept{
					return this->ptr!=other.ptr;
				}
				friend class Edges<Graph_t>::const_iterator;
		};
		class const_iterator{
			public:
				using Ptr=typename List<typename Graph_t::edge_handle>::const_iterator;
				using iterator_category=bidirectional_iterator_tag;
				using value_type=const typename Graph_t::edge_type;
				using pointer=typename Graph_t::const_edge_handle;
				using difference_type=ptrdiff_t;
				using reference=value_type&;
			private:
				Ptr ptr;
				size_t vertex;
			public:
				const_iterator(const iterator& nonconst_it)noexcept:ptr(nonconst_it.ptr),vertex(nonconst_it.vertex){}
				const_iterator(Ptr ptr,size_t vertex)noexcept:ptr(ptr),vertex(vertex){}
				constexpr reference operator*()const{
					return **ptr;
				}
				constexpr pointer operator->()const{
					return *ptr;
				}
				constexpr size_t follow()const{
					return (**ptr).follow_from(vertex);
				}
				constexpr const_iterator& operator++()noexcept{
					++ptr;
					return *this;
				}
				constexpr const_iterator operator++(int)noexcept{
					const_iterator aux=*this;
					++ptr;
					return aux;
				}
				constexpr const_iterator& operator--()noexcept{
					--ptr;
					return *this;
				}
				constexpr const_iterator operator--(int)noexcept{
					const_iterator aux=*this;
					--ptr;
					return aux;
				}
				constexpr bool operator==(const const_iterator& other)const noexcept{
					return this->ptr==other.ptr;
				}
				constexpr bool operator!=(const const_iterator& other)const noexcept{
					return this->ptr!=other.ptr;
				}
		};
		Edges(Graph_t& graph,size_t vertex)noexcept:graph(graph),vertex(vertex){}
		size_t size()const noexcept{
			return graph.vertexes[vertex].edges.size();
		}
		constexpr iterator begin()noexcept{
			return iterator(graph.vertexes[vertex].edges.begin(),vertex);
		}
		constexpr iterator end()noexcept{
			return iterator(graph.vertexes[vertex].edges.end(),vertex);
		}
		constexpr const_iterator begin()const noexcept{
			return const_iterator(graph.vertexes[vertex].edges.begin(),vertex);
		}
		constexpr const_iterator end()const noexcept{
			return const_iterator(graph.vertexes[vertex].edges.end(),vertex);
		}
	};
	template<typename Graph_t>
	class ConstEdges{
		const Graph_t& graph;
		size_t vertex;
	public:
		class iterator{
			public:
				using Ptr=typename List<typename Graph_t::edge_handle>::const_iterator;
				using iterator_category=bidirectional_iterator_tag;
				using value_type=const typename Graph_t::edge_type;
				using pointer=typename Graph_t::const_edge_handle;
				using difference_type=ptrdiff_t;
				using reference=const value_type&;
			private:
				Ptr ptr;
				size_t vertex;
			public:
				iterator(Ptr ptr,size_t vertex)noexcept:ptr(ptr),vertex(vertex){}
				constexpr reference operator*()const{
					return **ptr;
				}
				constexpr pointer operator->()const{
					return *ptr;
				}
				constexpr size_t follow()const{
					return (**ptr).follow_from(vertex);
				}
				constexpr iterator& operator++()noexcept{
					++ptr;
					return *this;
				}
				constexpr iterator operator++(int)noexcept{
					iterator aux=*this;
					++ptr;
					return aux;
				}
				constexpr iterator& operator--()noexcept{
					--ptr;
					return *this;
				}
				constexpr iterator operator--(int)noexcept{
					iterator aux=*this;
					--ptr;
					return aux;
				}
				constexpr bool operator==(const iterator& other)const noexcept{
					return this->ptr==other.ptr;
				}
				constexpr bool operator!=(const iterator& other)const noexcept{
					return this->ptr!=other.ptr;
				}
		};
		using const_iterator=iterator;
		ConstEdges(const Graph_t& graph,size_t vertex)noexcept:graph(const_cast<Graph_t&>(graph)),vertex(vertex){}
		size_t size()const noexcept{
			return graph.vertexes[vertex].edges.size();
		}
		constexpr const_iterator begin()const noexcept{
			return const_iterator(graph.vertexes[vertex].edges.begin(),vertex);
		}
		constexpr const_iterator end()const noexcept{
			return const_iterator(graph.vertexes[vertex].edges.end(),vertex);
		}
	};
	template<typename Graph_t>
	class Neighbors{
		const Graph_t& graph;
		size_t vertex;
	public:
		class iterator{
			public:
				using Ptr=typename List<typename Graph_t::edge_handle>::const_iterator;
				using iterator_category=bidirectional_iterator_tag;
				using value_type=size_t;
				using pointer=size_t;
				using difference_type=ptrdiff_t;
				using reference=size_t;
			private:
				Ptr ptr;
				size_t vertex;
			public:
				iterator(Ptr ptr,size_t vertex)noexcept:ptr(ptr),vertex(vertex){}
				constexpr value_type operator*()const{
					return (**ptr).follow_from(vertex);
				}
				constexpr iterator& operator++()noexcept{
					++ptr;
					return *this;
				}
				constexpr iterator operator++(int)noexcept{
					iterator aux=*this;
					++ptr;
					return aux;
				}
				constexpr iterator& operator--()noexcept{
					--ptr;
					return *this;
				}
				constexpr iterator operator--(int)noexcept{
					iterator aux=*this;
					--ptr;
					return aux;
				}
				constexpr bool operator==(const iterator& other)const noexcept{
					return this->ptr==other.ptr;
				}
				constexpr bool operator!=(const iterator& other)const noexcept{
					return this->ptr!=other.ptr;
				}
		};
		using const_iterator=iterator;
		Neighbors(const Graph_t& graph,size_t vertex)noexcept:graph(graph),vertex(vertex){}
		size_t size()const noexcept{
			return graph.vertexes[vertex].edges.size();
		}
		constexpr iterator begin()const noexcept{
			return iterator(graph.vertexes[vertex].edges.begin(),vertex);
		}
		constexpr iterator end()const noexcept{
			return iterator(graph.vertexes[vertex].edges.end(),vertex);
		}
	};

	template<typename Graph_t>
	class NeighborValues{
		Graph_t& graph;
		size_t vertex;
	public:
		class const_iterator;
		class iterator{
			public:
				using Ptr=typename List<typename Graph_t::edge_handle>::const_iterator;
				using iterator_category=bidirectional_iterator_tag;
				using value_type=typename Graph_t::vertex_value;
				using pointer=value_type*;
				using difference_type=ptrdiff_t;
				using reference=value_type&;
			private:
				Graph_t& graph;
				Ptr ptr;
				size_t vertex;
			public:
				iterator(Graph_t& graph,Ptr ptr,size_t vertex)noexcept:graph(graph),ptr(ptr),vertex(vertex){}
				constexpr reference operator*()const{
					return graph[(**ptr).follow_from(vertex)];
				}
				constexpr pointer operator->()const{
					return addressof(graph[(**ptr).follow_from(vertex)]);
				}
				constexpr iterator& operator++()noexcept{
					++ptr;
					return *this;
				}
				constexpr iterator operator++(int)noexcept{
					iterator aux=*this;
					++ptr;
					return aux;
				}
				constexpr iterator& operator--()noexcept{
					--ptr;
					return *this;
				}
				constexpr iterator operator--(int)noexcept{
					iterator aux=*this;
					--ptr;
					return aux;
				}
				constexpr bool operator==(const iterator& other)const noexcept{
					return this->ptr==other.ptr;
				}
				constexpr bool operator!=(const iterator& other)const noexcept{
					return this->ptr!=other.ptr;
				}
				friend class NeighborValues<Graph_t>::const_iterator;
		};
		class const_iterator{
			public:
				using Ptr=typename List<typename Graph_t::edge_handle>::const_iterator;
				using iterator_category=bidirectional_iterator_tag;
				using value_type=const typename Graph_t::vertex_value;
				using pointer=const value_type*;
				using difference_type=ptrdiff_t;
				using reference=const value_type&;
			private:
				const Graph_t& graph;
				Ptr ptr;
				size_t vertex;
			public:
				const_iterator(const iterator& nonconst_it)noexcept:graph(nonconst_it.graph),ptr(nonconst_it.ptr),vertex(nonconst_it.vertex){}
				const_iterator(const Graph_t& graph,Ptr ptr,size_t vertex)noexcept:graph(graph),ptr(ptr),vertex(vertex){}
				constexpr reference operator*()const{
					return graph[(**ptr).follow_from(vertex)];
				}
				constexpr pointer operator->()const{
					return addressof(graph[(**ptr).follow_from(vertex)]);
				}
				constexpr const_iterator& operator++()noexcept{
					++ptr;
					return *this;
				}
				constexpr const_iterator operator++(int)noexcept{
					const_iterator aux=*this;
					++ptr;
					return aux;
				}
				constexpr const_iterator& operator--()noexcept{
					--ptr;
					return *this;
				}
				constexpr const_iterator operator--(int)noexcept{
					const_iterator aux=*this;
					--ptr;
					return aux;
				}
				constexpr bool operator==(const const_iterator& other)const noexcept{
					return this->ptr==other.ptr;
				}
				constexpr bool operator!=(const const_iterator& other)const noexcept{
					return this->ptr!=other.ptr;
				}
		};
		NeighborValues(Graph_t& graph,size_t vertex)noexcept:graph(graph),vertex(vertex){}
		size_t size()const noexcept{
			return graph.vertexes[vertex].edges.size();
		}
		constexpr iterator begin()noexcept{
			return iterator(graph,graph.vertexes[vertex].edges.begin(),vertex);
		}
		constexpr iterator end()noexcept{
			return iterator(graph,graph.vertexes[vertex].edges.end(),vertex);
		}
		constexpr const_iterator begin()const noexcept{
			return const_iterator(graph,graph.vertexes[vertex].edges.begin(),vertex);
		}
		constexpr const_iterator end()const noexcept{
			return const_iterator(graph,graph.vertexes[vertex].edges.end(),vertex);
		}
	};
	template<typename Graph_t>
	class ConstNeighborValues{
		const Graph_t& graph;
		size_t vertex;
	public:
		class iterator{
			public:
				using Ptr=typename List<typename Graph_t::edge_handle>::const_iterator;
				using iterator_category=bidirectional_iterator_tag;
				using value_type=const typename Graph_t::vertex_value;
				using pointer=value_type*;
				using difference_type=ptrdiff_t;
				using reference=const value_type&;
			private:
				const Graph_t& graph;
				Ptr ptr;
				size_t vertex;
			public:
				iterator(const Graph_t& graph,Ptr ptr,size_t vertex)noexcept:graph(graph),ptr(ptr),vertex(vertex){}
				constexpr reference operator*()const{
					return graph[(**ptr).follow_from(vertex)];
				}
				constexpr pointer operator->()const{
					return addressof(graph[(**ptr).follow_from(vertex)]);
				}
				constexpr iterator& operator++()noexcept{
					++ptr;
					return *this;
				}
				constexpr iterator operator++(int)noexcept{
					iterator aux=*this;
					++ptr;
					return aux;
				}
				constexpr iterator& operator--()noexcept{
					--ptr;
					return *this;
				}
				constexpr iterator operator--(int)noexcept{
					iterator aux=*this;
					--ptr;
					return aux;
				}
				constexpr bool operator==(const iterator& other)const noexcept{
					return this->ptr==other.ptr;
				}
				constexpr bool operator!=(const iterator& other)const noexcept{
					return this->ptr!=other.ptr;
				}
		};
		using const_iterator=iterator;
		ConstNeighborValues(const Graph_t& graph,size_t vertex)noexcept:graph(graph),vertex(vertex){}
		size_t size()const noexcept{
			return graph.vertexes[vertex].edges.size();
		}
		constexpr const_iterator begin()const noexcept{
			return const_iterator(graph,graph.vertexes[vertex].edges.begin(),vertex);
		}
		constexpr const_iterator end()const noexcept{
			return const_iterator(graph,graph.vertexes[vertex].edges.end(),vertex);
		}
	};
	template<typename Out,typename EdgeHandle>
	Out& print_edges(Out& stream,size_t i,const List<EdgeHandle>& edges){
		stream<<"[";
		bool first=true;
		for(auto it=edges.begin();it!=edges.end();it++){
			if(!first)
				stream<<", ";
			stream<<""<<(*it)->follow_from(i);
			first=false;
		}
		stream<<"]";
		return stream;
	}
	template<typename Out,typename V,typename EdgeHandle>
	Out& print_vertexes(Out& stream,size_t i,const Vertex<V,EdgeHandle>& vertex){
		stream<<"["<<i;
		if constexpr(!mpv::is_void_v<V>) stream<<":"<<vertex.data;
		stream<<"] => ";
		print_edges<Out,EdgeHandle>(stream,i,vertex.edges);
		return stream;
	}
	template<typename Out,typename E>
	Out& operator<<(Out& stream,const Edge<E>& edge){
		stream<<"("<<edge.v<<","<<edge.u<<")";
		if constexpr(!mpv::is_void_v<E>) stream<<":"<<edge.data;
		return stream;
	}
	template<typename Out,typename V,typename E>
	Out& operator<<(Out& stream,const Graph<V,E>& graph){
		for(size_t i=0;i<graph.vertexes.get_internal_vector().size();i++){
			if(!graph.vertex_exist(i)) continue;
			print_vertexes(stream,i,graph.vertexes[i]);
			stream<<"\n";
		}
		stream<<"\n\nEdges: "<<graph.edges_list;
		return stream;
	}
}