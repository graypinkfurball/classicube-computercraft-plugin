for symbol in io.lines(arg[1]) do
  if symbol:sub(1,1) ~= "_" then
    print("#define "..symbol.." (*"..symbol..")")
  end
end

