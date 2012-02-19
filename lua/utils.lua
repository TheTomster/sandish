utils = {}

function utils.offBoard(x, y, z)
  local s = board.getSize()
  if
      x <= 0 or y <= 0 or z <= 0 or
      x > s or y > s or z > s then
    return true
  else
    return false
  end
end

return utils
