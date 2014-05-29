math.randomseed( os.time() )

function move( grid )
  return { x = math.random( 0, 7 ), y = math.random( 0, 7 ) }
end
