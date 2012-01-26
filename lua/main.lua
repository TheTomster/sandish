
function main()
  print("testing board.getE")
  print(board.getE(31, 31, 31))
  print("testing board.getN")
  print(board.getN(31, 31, 31))
  print("testing board.setN")
  print("should print 3")
  board.setN(0, 0, 0, 3)
  print(board.getN(0, 0, 0))
  print("testing tick")
  local exited = 0
  while exited == 0 do
    exited = tick.tick()
  end
end
