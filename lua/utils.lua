utils = {}

function utils.offBoard(x, y, z)
  local s = board.getSize()
  if
      x <= 0 or y <= 0 or z <= 0 or
      x > s - 1 or y > s - 1 or z > s - 1 then
    return true
  else
    return false
  end
end

return utils
