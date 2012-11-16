require("lua/utils")

MOVE_TIMER = 20
HASH_SIZE = 512

man = {}

assert(board.getSize() < HASH_SIZE, "Man particle requires board smaller than 512.")

local state = {}

local function hashFn(x, y, z)
  local x = x * HASH_SIZE * HASH_SIZE
  local y = y * HASH_SIZE
  local z = z
  return x + y + z
end

function man.init(dv)
  man.data_value = dv
end

function man.process(x, y, z)
  local s = man.getState(x, y, z)
  -- before the man even has a chance to update his timer, gravity strikes
  if man.falling(x, y, z) then
    man.move(x, y, z, x, y - 1, z, s)
  end
  -- he updates his timer
  s.timer = s.timer - 1
  -- he checks to see if it's time to move
  if s.timer > 0 then
    -- its not time to move. update state and wait
    man.setState(x, y, z, s)
    return
  end
  s.timer = MOVE_TIMER
  -- if he's carrying sand, he tries to head one space towards home
  if man.checkCarrySand(x, y, z) then
    local nx, ny, nz = man.attemptMoveToward(x, y, z, s.home.x, s.home.z, s)
    board.setN(x, y + 1, z, 0)
    board.setN(nx, ny + 1, nz, 1)
    if nx == s.home.x and nz == s.home.z then
      man.placeSand(nx, ny, nz, s)
    end
    return
  end
  -- if he's not carrying sand, but he remembers where to find some, he tries
  -- to head that way
  if s.last_seen_sand then
    local nx, ny, nz = man.attemptMoveToward(
        x, y, z, s.last_seen_sand.x, s.last_seen_sand.z, s)
    if
        nx == s.last_seen_sand.x and nz == s.last_seen_sand.z and
        not man.checkCarrySand(nx, ny, nz) then
      s.last_seen_sand = nil
    end
    return
  end
  -- otherwise he wanders aimlessly
  man.wander(x, y, z, s)
end

function man.placeSand(x, y, z, s)
  -- choose a suitable location
  local dx, dz
  if s.last_seen_sand then
    dx = utils.clamp(-1, 1, x - s.last_seen_sand.x)
    dz = utils.clamp(-1, 1, z - s.last_seen_sand.z)
  else
    dx = 0
    dz = 0
  end
  -- drop the sand
  board.setN(x, y + 1, z, 0)
  board.setN(x + dx, y + 1, z + dz, sandstone.data_value)
end

function man.falling(x, y, z)
  if board.getN(x, y - 1, z) == 0 then
    return true
  end
  return false
end

function man.move(fx, fy, fz, tx, ty, tz, s)
  board.setN(fx, fy, fz, 0)
  board.setN(tx, ty, tz, man.data_value)
  man.setState(fx, fy, fz, nil)
  man.setState(tx, ty, tz, s)
end

function man.attemptMoveToward(fx, fy, fz, tx, tz, s)
  -- check if there's sand nearby
  if not man.checkCarrySand(fx, fy, fz) then
    for x = -1, 1 do
      for z = -1, 1 do
        if board.getE(fx + x, fy, fz + z) == sand.data_value then
          man.carrySand(fx, fy, fz, fx + x, fy, fz + z, s)
          man.setState(fx, fy, fz, s)
          return fx, fy, fz
        end
      end
    end
  end
  -- no sand nearby, so move towards destination
  local dx = utils.clamp(-1, 1, tx - fx)
  local dz = utils.clamp(-1, 1, tz - fz)
  local nx, nz = fx + dx, fz + dz
  if board.getE(nx, fy, nz) == 0 and board.getN(nx, fy, nz) == 0 then
    if not utils.offBoard(nx, fy, nz) then
      man.move(fx, fy, fz, nx, fy, nz, s)
      return nx, fy, nz
    end
  elseif
      board.getE(nx, fy + 1, nz) == 0 and board.getN(nx, fy + 1, nz) == 0 then
    -- try climbing
    if not utils.offBoard(nx, fy + 1, nz) then
      man.move(fx, fy, fz, nx, fy + 1, nz, s)
      return nx, fy + 1, nz
    end
  end
  return fx, fy, fz
end

function man.carrySand(x, y, z, sx, sy, sz, s)
  board.setN(sx, sy, sz, 0)
  board.setN(x, y + 1, z, sand.data_value)
  s.last_seen_sand = {x = sx, y = sy, z = sz}
  man.move(x, y, z, x, y, z, s)
end

function man.wander(x, y, z, s)
  local dx = math.random(-1, 1)
  local dz = math.random(-1, 1)
  local nx = x + dx
  local nz = z + dz
  man.attemptMoveToward(x, y, z, nx, nz, s)
end

function man.checkCarrySand(x, y, z)
  y = y + 1
  if board.getE(x, y, z) == sand.data_value then
    return true
  end
  return false
end

function man.getState(x, y, z)
  local key = hashFn(x, y, z)

  if not state[key] then
    state[key] = man.newState(x, y, z)
  end

  return state[key]
end

function man.setState(x, y, z ,s)
  local key = hashFn(x, y, z)
  state[key] = s
end

function man.newState(x, y, z)
  local s = {}
  s.home = {x = x, z = z}
  s.timer = MOVE_TIMER
  return s
end

return man
