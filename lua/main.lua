rules = {}

function newRule(shader, script)
  local handle = registry.add(shader)
  local rule_chunk, err = loadfile(script)
  if err ~= nil then
    print("Error loading " .. script)
    error(err)
  end
  ok, rule = pcall(rule_chunk)
  if not ok then
    print("Error running " .. script)
    error(rule)
  end
  ok, err = pcall(rule.init, handle)
  if not ok then
    print("Error running init(handle) in " .. script)
    error(err)
  end
  rules[handle] = rule
  return handle
end

function registry.init()
  d1 = newRule("res/debug.frag.glsl", "lua/debug.lua")
end


function main()
  -- Enter mainloop
  local exited = 0
  local timer = tick.getTime()
  while exited == 0 do
    local time_up = (tick.getTime() - timer) > (1.0/30.0)
    if time_up then
      timer = tick.getTime()
      board.setN(64, 128, 64, d1)
      for x = 1, board.size do
        for y = 1, board.size do
          for z = 1, board.size do
            local d = board.getE(x, y, z)
            if d ~= 0 then
              ok, err = pcall(rules[d].process, x, y, z)
            end
          end
        end
      end
    end
    exited = tick.tick(time_up)
  end
end
