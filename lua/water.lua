require("utils")

water = {}

function water.init(dv)
  water.data_value = dv
end

function water.process(x, y, z)
  local e = board.getE(x, y - 1, z)
  local n = board.getN(x, y - 1, z)
  local off_board = utils.offBoard(x, y - 1, z)
  -- Pick a random direction and try that too
  local xr = x + math.random(-1, 1)
  local yr = y + math.random(-2, 0)
  local zr = z + math.random(-1, 1)
  local er = board.getE(xr, yr, zr)
  local nr = board.getN(xr, yr, zr)
  local off_board_r = utils.offBoard(xr, yr, zr)
  if e == 0 and n == 0 and not off_board then
    board.setN(x, y, z, 0)
    board.setN(x, y - 1, z, water.data_value)
  elseif er == 0 and nr == 0 and not off_board_r then
    board.setN(x, y, z, 0)
    board.setN(xr, yr, zr, water.data_value)
  end
end

return water
