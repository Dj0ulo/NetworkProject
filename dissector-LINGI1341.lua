course_proto = Proto("lingi1341", "LINGI1341 Transmission Protocol")
function course_proto.dissector(buffer,pinfo,tree)
    pinfo.cols.protocol = "LINGI1341"
    local subtree = tree:add(course_proto,buffer(),
        "LINGI1341 Transmission Protocol Data")
    
    if (buffer:len() < 11) then
        subtree:add(buffer(0, buffer:len()), "Packet is too short (no header)!")
    else
        local header = subtree:add(buffer(0, 4), "Header")
        local ptype = buffer(0, 1):bitfield(0, 2)
        local ptype_text = "UNKNOWN"
        if (ptype == 1) then
            ptype_text = "PTYPE_DATA"
        elseif (ptype == 2) then
            ptype_text = "PTYPE_ACK"
        elseif (ptype == 3) then
            ptype_text = "PTYPE_NACK"
        end
        header:add(buffer(0, 1),"PTYPE: " .. ptype_text)
        header:add(buffer(0, 1), "TR: " .. buffer(0, 1):bitfield(2, 1))
        header:add(buffer(0, 1),"Window: " .. buffer(0, 1):bitfield(3, 5))
        local len_size = buffer(1, 1):bitfield(0, 1) + 1
        local len = buffer(1, len_size):bitfield(1, len_size*8-1)
        header:add(buffer(1, len_size),"Payload Length Size: " .. len_size)
        header:add(buffer(1, len_size),"Payload Length: " .. len)
        local after_len_idx = 1 + len_size
        header:add(buffer(after_len_idx, 1),"Sequence number: " .. buffer(after_len_idx, 1):uint())
        header:add(buffer(after_len_idx + 1, 4),"Timestamp: " .. buffer(after_len_idx + 1,4):uint())
        header:add(buffer(after_len_idx + 1 + 4, 4),"CRC1: " .. buffer(after_len_idx + 1 + 4,4):uint())
        
        if (buffer:len() > 10 + len_size and buffer:len() ~= 10 + len_size + 4 + len) then
            subtree:add(buffer(0, buffer:len()), "Packet is inconsistent!")        
        elseif (buffer:len() > 528) then
            subtree:add(buffer(0, buffer:len()), "Packet is too long!")
        elseif (buffer:len() > 11) then
            subtree:add(buffer(10 + len_size, len), "Payload [" .. len .. " bytes]")
            subtree:add(buffer(buffer:len()-4, 4),
                        "CRC2: " .. buffer(buffer:len()-4, 4):uint())
        end
    end
end

udp_table = DissectorTable.get("udp.port")
udp_table:add(64341, course_proto)
udp_table:add(64342, course_proto)
