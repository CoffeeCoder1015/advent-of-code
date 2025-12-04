let in_chan = open_in "test.txt" in 
  let len = in_channel_length in_chan in
  let grid = really_input_string in_chan len in 
  Printf.printf "%s\n" grid