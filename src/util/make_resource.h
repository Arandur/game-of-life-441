#pragma once

template < typename Creator, typename Destructor, typename... Args >
auto make_resource( Creator c, Destructor d, Args&&... args ) {
  typedef typename std::decay< decltype( *c( std::forward< Args >( args )... ) ) >::type ResourceType;
  return std::unique_ptr< ResourceType, void(*)( ResourceType* ) >( c( std::forward< Args >( args )... ), d );
}