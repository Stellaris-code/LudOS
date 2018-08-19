#ifndef kpp_DETAIL_STRING_VIEW_INL
#define kpp_DETAIL_STRING_VIEW_INL

#include <string.h>
#include <assert.h>

namespace kpp {

  //--------------------------------------------------------------------------
  // Constructor
  //--------------------------------------------------------------------------

  template<typename CharT>
  inline constexpr basic_string_view<CharT>::basic_string_view()
    noexcept
    : m_str(nullptr),
      m_size(0)
  {

  }

  template<typename CharT>
  inline constexpr basic_string_view<CharT>::basic_string_view( const kpp::string& str )
    noexcept
    : m_str(str.c_str()),
      m_size(str.size())
  {

  }

  template<typename CharT>
  inline constexpr basic_string_view<CharT>::basic_string_view( const char_type* str )
    noexcept
    : m_str(str),
      m_size(strlen(str))
  {

  }

  template<typename CharT>
  inline constexpr basic_string_view<CharT>::basic_string_view( const char_type* str, size_type count )
    noexcept
    : m_str(str),
      m_size(count)
  {

  }

  //--------------------------------------------------------------------------
  // Capacity
  //--------------------------------------------------------------------------

  template<typename CharT>
  inline constexpr typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::size()
    const noexcept
  {
    return m_size;
  }

  template<typename CharT>
  inline constexpr typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::length()
    const noexcept
  {
    return size();
  }

  template<typename CharT>
  inline constexpr typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::max_size()
    const noexcept
  {
    return npos - 1;
  }

  template<typename CharT>
  inline constexpr bool basic_string_view<CharT>::empty()
    const noexcept
  {
    return m_size == 0;
  }

  //--------------------------------------------------------------------------
  // Element Access
  //--------------------------------------------------------------------------

  template<typename CharT>
  inline constexpr const typename basic_string_view<CharT>::char_type*
    basic_string_view<CharT>::c_str()
    const noexcept
  {
    return m_str;
  }

  template<typename CharT>
  inline constexpr const typename basic_string_view<CharT>::char_type*
    basic_string_view<CharT>::data()
    const noexcept
  {
    return m_str;
  }

  template<typename CharT>
  inline constexpr typename basic_string_view<CharT>::const_reference
    basic_string_view<CharT>::operator[]( size_t pos )
    const noexcept
  {
    return m_str[pos];
  }

  template<typename CharT>
  inline constexpr typename basic_string_view<CharT>::const_reference
    basic_string_view<CharT>::at( size_t pos )
    const
  {
    assert(pos < m_size);
    return m_str[pos];
  }

  template<typename CharT>
  inline constexpr typename basic_string_view<CharT>::const_reference
    basic_string_view<CharT>::front( )
    const noexcept
  {
    return *m_str;
  }

  template<typename CharT>
  inline constexpr typename basic_string_view<CharT>::const_reference
    basic_string_view<CharT>::back( )
    const noexcept
  {
    return m_str[m_size-1];
  }

  //--------------------------------------------------------------------------
  // Modifiers
  //--------------------------------------------------------------------------

  template<typename CharT>
  inline void
    basic_string_view<CharT>::remove_prefix( size_type n )
    noexcept
  {
    m_str += n, m_size -= n;
  }

  template<typename CharT>
  inline void
    basic_string_view<CharT>::remove_suffix( size_type n )
    noexcept
  {
    m_size -= n;
  }

  template<typename CharT>
  inline void
    basic_string_view<CharT>::swap( basic_string_view& v )
    noexcept
  {
    using std::swap;
    swap(m_size,v.m_size);
    swap(m_str,v.m_str);
  }

  //--------------------------------------------------------------------------
  // Conversions
  //--------------------------------------------------------------------------

  template<typename CharT>
  inline constexpr kpp::string
    basic_string_view<CharT>::to_string()
    const
  {
    return kpp::string( m_str, m_size);
  }

  template<typename CharT>
  inline constexpr basic_string_view<CharT>::operator
    kpp::string()
    const
  {
    return kpp::string( m_str, m_size );
  }

  //--------------------------------------------------------------------------
  // String Operations
  //--------------------------------------------------------------------------

  template<typename CharT>
  inline typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::copy( char_type* dest,
                                           size_type count,
                                           size_type pos )
    const
  {
    assert(pos < m_size);

    const size_type rcount = std::min(m_size - pos,count+1);
    std::copy( m_str + pos, m_str + pos + rcount, dest);
    return rcount;
  }

  template<typename CharT>
  inline basic_string_view<CharT>
    basic_string_view<CharT>::substr( size_t pos,
                                             size_t len )
    const
  {
    const size_type max_length = pos > m_size ? 0 : m_size - pos;
    assert(pos < m_size);

    return basic_string_view<CharT>( m_str + pos, len > max_length ? max_length : len );
  }

  //--------------------------------------------------------------------------

  template<typename CharT>
  inline int basic_string_view<CharT>::compare( basic_string_view v )
    const noexcept
  {
    const size_type rlen = std::min(m_size,v.m_size);
    const int compare = strncmp(m_str,v.m_str,rlen);

    return (compare ? compare : (m_size < v.m_size ? -1 : (m_size > v.m_size ? 1 : 0)));
  }

  template<typename CharT>
  inline int basic_string_view<CharT>::compare( size_type pos,
                                                       size_type count,
                                                       basic_string_view v )
    const
  {
    return substr(pos,count).compare(v);
  }

  template<typename CharT>
  inline int basic_string_view<CharT>::compare( size_type pos1,
                                                       size_type count1,
                                                       basic_string_view v,
                                                       size_type pos2,
                                                       size_type count2 )
    const
  {
    return substr(pos1,count1).compare( v.substr(pos2,count2) );
  }

  template<typename CharT>
  inline int basic_string_view<CharT>::compare( const char_type* s )
    const
  {
    return compare(basic_string_view<CharT>(s));
  }

  template<typename CharT>
  inline int basic_string_view<CharT>::compare( size_type pos,
                                                       size_type count,
                                                       const char_type* s )
    const
  {
    return substr(pos, count).compare( basic_string_view<CharT>(s) );
  }

  template<typename CharT>
  inline int basic_string_view<CharT>::compare( size_type pos,
                                                       size_type count1,
                                                       const char_type* s,
                                                       size_type count2 )
    const
  {
    return substr(pos, count1).compare( basic_string_view<CharT>(s, count2) );
  }

  //--------------------------------------------------------------------------

  template<typename CharT>
  inline typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::find( basic_string_view v,
                                           size_type pos )
    const
  {
    const size_type max_index = m_size - v.size();

    for( size_type i = pos; i < max_index; ++i ) {
      size_type j = v.size()-1;
      for( ; j >= 0; --j ) {
        if( v[j] != m_str[i+j] ) {
          break;
        }
      }
      if((j+1)==0) return i;
    }
    return npos;
  }

  template<typename CharT>
  inline typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::find( char_type c,
                                           size_type pos )
    const
  {
    return find(basic_string_view<CharT>(&c, 1), pos);
  }

  template<typename CharT>
  inline typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::find( const char_type* s, size_type pos,
                                           size_type count )
    const
  {
    return find(basic_string_view<CharT>(s, count), pos);
  }

  template<typename CharT>
  inline typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::find( const char_type* s,
                                          size_type pos )
    const
  {
    return find(basic_string_view<CharT>(s), pos);
  }

  //--------------------------------------------------------------------------

  template<typename CharT>
  inline typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::rfind( basic_string_view v,
                                            size_type pos )
    const
  {
    const size_type max_index = m_size - v.size();

    for( size_type i = std::min(max_index-1,pos); i >= 0; --i ) {
      size_type j = 0;
      for( ; j < v.size(); ++j ) {
        if( v[j] != m_str[i-j] ) {
          break;
        }
      }
      if(j==v.size()) return i;
    }
    return npos;
  }

  template<typename CharT>
  inline typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::rfind( char_type c,
                                           size_type pos )
    const
  {
    return rfind(basic_string_view<CharT>(&c, 1), pos);
  }

  template<typename CharT>
  inline typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::rfind( const char_type* s, size_type pos,
                                           size_type count )
    const
  {
    return rfind(basic_string_view<CharT>(s, count), pos);
  }

  template<typename CharT>
  inline typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::rfind( const char_type* s,
                                          size_type pos )
    const
  {
    return rfind(basic_string_view<CharT>(s), pos);
  }

  //--------------------------------------------------------------------------

  template<typename CharT>
  inline typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::find_first_of( basic_string_view v,
                                                    size_type pos )
    const
  {
    for( size_type i = pos; i < m_size; ++i ) {
      for( size_type j = 0; j < v.size(); ++j ) {
        if( v[j] == m_str[i] ) {
          return i;
        }
      }
    }
    return npos;
  }

  template<typename CharT>
  inline typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::find_first_of( char_type c,
                                                    size_type pos )
    const
  {
    return find_first_of(basic_string_view<CharT>(&c, 1), pos);
  }

  template<typename CharT>
  inline typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::find_first_of( const char_type* s, size_type pos,
                                                    size_type count )
    const
  {
    return find_first_of(basic_string_view<CharT>(s, count), pos);
  }

  template<typename CharT>
  inline typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::find_first_of( const char_type* s,
                                                    size_type pos )
    const
  {
    return find_first_of( basic_string_view<CharT>(s), pos );
  }

  //--------------------------------------------------------------------------

  template<typename CharT>
  inline typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::find_last_of( basic_string_view v,
                                                   size_type pos )
    const
  {
    for( size_type i = std::min(m_size-1,pos); i >= 0; --i ) {
      for( size_type j = 0; j < v.size(); ++j ) {
        if( v[j] == m_str[i] ) {
          return i;
        }
      }
    }
    return npos;
  }

  template<typename CharT>
  inline typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::find_last_of( char_type c,
                                                   size_type pos )
    const
  {
    return find_last_of( basic_string_view<CharT>(&c, 1), pos );
  }

  template<typename CharT>
  inline typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::find_last_of( const char_type* s, size_type pos,
                                                   size_type count )
    const
  {
    return find_last_of( basic_string_view<CharT>(s, count), pos );
  }

  template<typename CharT>
  inline typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::find_last_of( const char_type* s,
                                                   size_type pos )
    const
  {
    return find_last_of( basic_string_view<CharT>(s), pos );
  }

  //--------------------------------------------------------------------------

  template<typename CharT>
  inline typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::find_first_not_of( basic_string_view v,
                                                        size_type pos )
    const
  {
    for( size_type i = pos; i < m_size; ++i ) {
      for( size_type j = 0; j < v.size(); ++j ) {
        if( v[j] == m_str[i] ) {
          break;
        }
        return i;
      }
    }
    return npos;
  }

  template<typename CharT>
  inline typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::find_first_not_of( char_type c,
                                                        size_type pos )
    const
  {
    return find_first_not_of( basic_string_view<CharT>(&c, 1), pos );
  }

  template<typename CharT>
  inline typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::find_first_not_of( const char_type* s, size_type pos,
                                                        size_type count )
    const
  {
    return find_first_not_of( basic_string_view<CharT>(s, count), pos );
  }

  template<typename CharT>
  inline typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::find_first_not_of( const char_type* s,
                                                        size_type pos )
    const
  {
    return find_first_not_of( basic_string_view<CharT>(s), pos );
  }

  //--------------------------------------------------------------------------

  template<typename CharT>
  inline typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::find_last_not_of( basic_string_view v,
                                                       size_type pos )
    const
  {
    for( size_type i = std::min(m_size-1,pos); i >= 0; --i ) {
      for( size_type j = 0; j < v.size(); ++j ) {
        if( v[j] == m_str[i] ) {
          break;
        }
        return i;
      }
    }
    return npos;
  }

  template<typename CharT>
  inline typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::find_last_not_of( char_type c,
                                                       size_type pos )
    const
  {
    return find_last_not_of( basic_string_view<CharT>(&c, 1), pos );
  }

  template<typename CharT>
  inline typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::find_last_not_of( const char_type* s, size_type pos,
                                                       size_type count )
    const
  {
    return find_last_not_of( basic_string_view<CharT>(s, count), pos );
  }

  template<typename CharT>
  inline typename basic_string_view<CharT>::size_type
    basic_string_view<CharT>::find_last_not_of( const char_type* s,
                                                       size_type pos )
    const
  {
    return find_last_not_of( basic_string_view<CharT>(s), pos );
  }

  //--------------------------------------------------------------------------
  // Iterator
  //--------------------------------------------------------------------------

  template<typename CharT>
  inline typename basic_string_view<CharT>::const_iterator
    basic_string_view<CharT>::begin()
    const noexcept
  {
    return m_str;
  }

  template<typename CharT>
  inline typename basic_string_view<CharT>::const_iterator
    basic_string_view<CharT>::end()
    const noexcept
  {
    return m_str + m_size;
  }

  template<typename CharT>
  inline typename basic_string_view<CharT>::const_iterator
    basic_string_view<CharT>::cbegin()
    const noexcept
  {
    return m_str;
  }

  template<typename CharT>
  inline typename basic_string_view<CharT>::const_iterator
    basic_string_view<CharT>::cend()
    const noexcept
  {
    return m_str + m_size;
  }

  //--------------------------------------------------------------------------
  // Public Functions
  //--------------------------------------------------------------------------

  template<typename CharT>
  inline void swap( basic_string_view<CharT>& lhs,
                    basic_string_view<CharT>& rhs )
    noexcept
  {
    lhs.swap(rhs);
  }

  //--------------------------------------------------------------------------
  // Comparison Functions
  //--------------------------------------------------------------------------

  template<typename CharT>
  inline bool operator == ( const basic_string_view<CharT>& lhs,
                            const basic_string_view<CharT>& rhs )
    noexcept
  {
    return lhs.compare(rhs) == 0;
  }

  template<typename CharT>
  inline bool operator == ( basic_string_view<CharT> lhs,
                            const CharT* rhs )
    noexcept
  {
    return lhs == basic_string_view<CharT>(rhs);
  }

  template<typename CharT>
  inline bool operator == ( const CharT* lhs,
                            const basic_string_view<CharT>& rhs )
    noexcept
  {
    return basic_string_view<CharT>(lhs) == rhs;
  }

  template<typename CharT>
  inline bool operator == ( const kpp::string& lhs,
                            const basic_string_view<CharT>& rhs )
  {
    return basic_string_view<CharT>(lhs) == rhs;
  }

  template<typename CharT>
  inline bool operator == ( const basic_string_view<CharT>& lhs,
                            const kpp::string& rhs )
  {
    return lhs == basic_string_view<CharT>(rhs);
  }

  //--------------------------------------------------------------------------

  template<typename CharT>
  inline bool operator != ( const basic_string_view<CharT>& lhs,
                            const basic_string_view<CharT>& rhs )
    noexcept
  {
    return lhs.compare(rhs) != 0;
  }

  template<typename CharT>
  inline bool operator != ( const basic_string_view<CharT>& lhs,
                            const CharT* rhs )
    noexcept
  {
    return lhs != basic_string_view<CharT>(rhs);
  }

  template<typename CharT>
  inline bool operator != ( const CharT* lhs,
                            const basic_string_view<CharT>& rhs )
    noexcept
  {
    return basic_string_view<CharT>(lhs) != rhs;
  }

  template<typename CharT>
  inline bool operator != ( const kpp::string& lhs,
                            const basic_string_view<CharT>& rhs )
  {
    return basic_string_view<CharT>(lhs) != rhs;
  }

  template<typename CharT>
  inline bool operator != ( const basic_string_view<CharT>& lhs,
                            const kpp::string& rhs )
  {
    return lhs != basic_string_view<CharT>(rhs);
  }
  //--------------------------------------------------------------------------

  template<typename CharT>
  inline bool operator < ( const basic_string_view<CharT>& lhs,
                           const basic_string_view<CharT>& rhs )
    noexcept
  {
    return lhs.compare(rhs) < 0;
  }

  template<typename CharT>
  inline bool operator < ( const basic_string_view<CharT>& lhs,
                           const CharT* rhs )
    noexcept
  {
    return lhs < basic_string_view<CharT>(rhs);
  }

  template<typename CharT>
  inline bool operator < ( const CharT* lhs,
                           const basic_string_view<CharT>& rhs )
    noexcept
  {
    return basic_string_view<CharT>(lhs) < rhs;
  }

  template<typename CharT>
  inline bool operator < ( const kpp::string& lhs,
                           const basic_string_view<CharT>& rhs )
  {
    return basic_string_view<CharT>(lhs) < rhs;
  }

  template<typename CharT>
  inline bool operator < ( const basic_string_view<CharT>& lhs,
                           const kpp::string& rhs )
  {
    return lhs < basic_string_view<CharT>(rhs);
  }

  //--------------------------------------------------------------------------

  template<typename CharT>
  inline bool operator > ( const basic_string_view<CharT>& lhs,
                           const basic_string_view<CharT>& rhs )
    noexcept
  {
    return lhs.compare(rhs) > 0;
  }

  template<typename CharT>
  inline bool operator > ( const basic_string_view<CharT>& lhs,
                           const CharT* rhs )
    noexcept
  {
    return lhs > basic_string_view<CharT>(rhs);
  }

  template<typename CharT>
  inline bool operator > ( const CharT* lhs,
                           const basic_string_view<CharT>& rhs )
    noexcept
  {
    return basic_string_view<CharT>(lhs) > rhs;
  }

  template<typename CharT>
  inline bool operator > ( const kpp::string& lhs,
                           const basic_string_view<CharT>& rhs )
  {
    return basic_string_view<CharT>(lhs) > rhs;
  }

  template<typename CharT>
  inline bool operator > ( const basic_string_view<CharT>& lhs,
                           const kpp::string& rhs )
  {
    return lhs > basic_string_view<CharT>(rhs);
  }

  //--------------------------------------------------------------------------

  template<typename CharT>
  inline bool operator <= ( const basic_string_view<CharT>& lhs,
                            const basic_string_view<CharT>& rhs )
    noexcept
  {
    return lhs.compare(rhs) <= 0;
  }

  template<typename CharT>
  inline bool operator <= ( const basic_string_view<CharT>& lhs,
                            const CharT* rhs )
    noexcept
  {
    return lhs <= basic_string_view<CharT>(rhs);
  }

  template<typename CharT>
  inline bool operator <= ( const CharT* lhs,
                            const basic_string_view<CharT>& rhs )
    noexcept
  {
    return basic_string_view<CharT>(lhs) <= rhs;
  }

  template<typename CharT>
  inline bool operator <= ( const kpp::string& lhs,
                            const basic_string_view<CharT>& rhs )
  {
    return basic_string_view<CharT>(lhs) <= rhs;
  }

  template<typename CharT>
  inline bool operator <= ( const basic_string_view<CharT>& lhs,
                            const kpp::string& rhs )
  {
    return lhs <= basic_string_view<CharT>(rhs);
  }

  //--------------------------------------------------------------------------

  template<typename CharT>
  inline bool operator >= ( const basic_string_view<CharT>& lhs,
                            const basic_string_view<CharT>& rhs )
    noexcept
  {
    return lhs.compare(rhs) >= 0;
  }

  template<typename CharT>
  inline bool operator >= ( const basic_string_view<CharT>& lhs,
                            const CharT* rhs )
    noexcept
  {
    return lhs >= basic_string_view<CharT>(rhs);
  }

  template<typename CharT>
  inline bool operator >= ( const CharT* lhs,
                            const basic_string_view<CharT>& rhs )
    noexcept
  {
    return basic_string_view<CharT>(lhs) >= rhs;
  }

  template<typename CharT>
  inline bool operator >= ( const kpp::string& lhs,
                            const basic_string_view<CharT>& rhs )
  {
    return basic_string_view<CharT>(lhs) >= rhs;
  }

  template<typename CharT>
  inline bool operator >= ( const basic_string_view<CharT>& lhs,
                            const kpp::string& rhs )
  {
    return lhs >= basic_string_view<CharT>(rhs);
  }

} // namespace kpp

#endif /* kpp_DETAIL_STRING_VIEW_INL */
