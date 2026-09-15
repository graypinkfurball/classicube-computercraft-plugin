local is_cc_symbol = {}
for symbol in io.lines(arg[1]) do
  if symbol:sub(1,1) ~= "_" then
    is_cc_symbol[symbol] = true
  end
end

local symbols = {}
for symbol in io.lines(arg[2]) do
  if is_cc_symbol[symbol] then
    table.insert(symbols, symbol)
  end
end

print("void *"..table.concat(symbols, ",*")..";")
print("static char *names[] = {\""..table.concat(symbols, "\",\"").."\",(char *)0};")
print("static void **addrs[] = {&"..table.concat(symbols, ",&").."};")


