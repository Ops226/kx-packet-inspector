local table_base = 0x000002BE68192490
local table_size = 521
local entry_size = 16
local schema_offset = 8

print("--- CMSG Schema Table Dump ---")
for i = 0, table_size - 1 do
  local entry_addr = table_base + (i * entry_size)
  local schema_ptr_addr = entry_addr + schema_offset
  
  -- readPointer reads an 8-byte value
  local schema_addr = readPointer(schema_ptr_addr)
  
  if schema_addr and schema_addr ~= 0 then
    print(string.format("Opcode: 0x%04X -> Schema Address: 0x%X", i, schema_addr))
  end
end
print("--- Dump Complete ---")