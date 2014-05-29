function mutate( grid )
  local r = #grid
  local c = #grid[1]
  local new_grid = {}

  -- Copy cell values
  for i = 1, r do
    new_grid[i] = {}
    for j = 1, c do
      new_grid[i][j] = grid[i][j]
    end
  end

  for x = 1, r do
    for y = 1, c do
      local neighbors = { [0] = 0, [1] = 0, [2] = 0 }
      for i = -1, 1 do
        for j = -1, 1 do
          if i ~= 0 or j ~= 0 then
            if not ( x == 1 and i == -1 ) and x + i <= r and
               not ( y == 1 and j == -1 ) and y + j <= c then
              neighbors[ grid[ x + i ][ y + j ] ] = 
                neighbors[ grid[ x + i ][ y + j ] ] + 1
            else
              neighbors[0] = neighbors[0] + 1
            end
          end
        end
      end

      if neighbors[0] == 5 or ( grid[x][y] ~= 0 and neighbors[0] == 6 ) then
        if neighbors[1] > neighbors[2] then
          new_grid[x][y] = 1
        elseif neighbors[1] < neighbors[2] then
          new_grid[x][y] = 2
        end
      else
        new_grid[x][y] = 0
      end
    end
  end

  return new_grid
end
