#pragma once

#include <cstddef>
#include <cstring>

/**
 * \brief Memory-safe two-dimensional array class
 *
 * This class is, very simply, a two-dimensional non-resizable array. It
 * have any fancy features like begin() and end() overloads, so it's not a
 * complete container, but it suffices for the sake of this project. The
 * task of extending its functionality (e.g. resizing, iteration ) is left
 * as an exercise for the reader.
 */ 
template < class T >
class Grid {
  // This class is here for memory safety, nothing more. It just encapsulates
  // a row.
  class Row {
   public:
    Row() :
      size( 0 ), data( nullptr ) {}
    
    Row( const Row& r ) :
      size( r.size ), data( new T[ size ] ) {
      memcpy( r.data, data, size );
    }

    Row( Row&& r ) :
      size( r.size ), data( r.data ) {
      r.size = 0;
      r.data = nullptr;
    }
    
    
    Row( const size_t& s, T* d ) :
      size( s ), data( d ) {}

    ~Row() {}

    T& operator [] ( const size_t& i ) {
      if( i < size )
        return data[i];
      else
        throw std::out_of_range( nullptr );
    }

    const T& operator [] ( const size_t& i ) const {
      if( i < size )
        return data[i];
      else
        throw std::out_of_range( nullptr );
    }
   private:
    size_t size;
    T* data;
  };

 public:
  /**
   * \brief Default constructor.
   * 
   * Constructs a zero by zero Grid. Since Grids are non-resizable, this is
   * a bit useless, but it's here for completeness.
   */
  Grid() :
    rows( 0 ), cols( 0 ), data( nullptr ) {}

  /**
   * \brief Copy constructor.
   */
  Grid( const Grid& g ) :
    rows( g.rows ), cols( g.cols ), data( new T[ rows * cols ] ) {
    memcpy( g.data, data, rows * cols );
  }

  /**
   * \brief Move constructor.
   */
  Grid( Grid&& g ) :
    rows( g.rows ), cols( g.cols ), data( g.data ) {
    g.rows = 0;
    g.cols = 0;
    g.data = nullptr;
  }

  /**
   * \brief Size constructor.
   *
   * This is the only constructor you need to worry about; it creates a Grid
   * of a given width and height.
   *
   * \param r Number of rows in the Grid
   * \param c Number of columns in the Grid
   */
  Grid( const size_t& r, const size_t& c ) :
    rows( r ), cols( c ), data( new T[ rows * cols ] ) {
    std::fill_n( data, rows * cols, T() );  
  }

  /**
   * \brief Default destructor.
   */
  ~Grid() {
    if( data ) delete[] data;
  }

  /**
   * \brief Assignment operator.
   */
  Grid& operator = ( const Grid& g ) {
    rows = g.rows;
    cols = g.cols;
    if( data ) delete[] data;
    data = new T[ rows * cols ];
    for( size_t i = 0; i < rows; ++i )
      for( size_t j = 0; j < rows; ++j )
        (*this)[i][j] = g[i][j];

    return *this;
  }

   /**
    * \brief Move assignment operator
    */
  Grid& operator = ( Grid&& g ) {
    rows = g.rows;
    cols = g.cols;
    if( data ) delete[] data;
    data = g.data;
    g.rows = 0;
    g.cols = 0;
    g.data = nullptr;
  }

  /**
   * \brief Random access operator
   *
   * Returns a row which in turn overloads operator [] in order to mimic
   * a two-dimensional C-style array, but with bounds-checking. Elements
   * in the array support assignment as well as read access.
   */
  Row operator [] ( const size_t& i ) {
    if( i < rows )
      return Row( cols, data + i * cols );
    else
      throw std::out_of_range( "" );
  }

  /**
   * \brief Random access read operator
   */
  const Row operator [] ( const size_t& i ) const {
    if( i < rows )
      return Row( cols, data + i * cols );
    else
      throw std::out_of_range( "" );
  }

  /**
   * \return Number of rows in the Grid.
   */
  size_t nRows() const {
    return rows;
  }

  /**
   * \return Number of columns in the Grid.
   */
  size_t nCols() const {
    return cols;
  }

 private:
  size_t rows, cols;
  T* data;
};
