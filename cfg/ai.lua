math.randomseed( os.time() )

function move( grid )
  return { x = math.random( 0, #grid ), y = math.random( 0, #grid[1] ) }
end
