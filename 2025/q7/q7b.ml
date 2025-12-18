let in_chan = open_in "test.txt" in 
  let first_line = input_line in_chan in
  let n = String.length first_line in
  let start_idx = String.index first_line 'S' in
  let split_counter = Array.make n 0 in
  split_counter.(start_idx) <- 1;
  let splitter idx = 
    let l = idx-1 in 
    let u = idx+1 in
    let current = split_counter.(idx) in
    if l >= 0 then
      split_counter.(l) <- split_counter.(l) + current ;
    if u < n then
      split_counter.(u) <- split_counter.(u) + current ;
    split_counter.(idx) <- 0;
  in
  let rec line_consumer i = 
    if (i mod 2 = 1) then
      let _ = input_line in_chan in
      line_consumer (i+1)
    else
    match input_line in_chan  with
    | line -> 
        let act_idx = Seq.filter_map (fun (i,c) -> if c = '^' then Some i else None) ( String.to_seqi line ) 
        |> List.of_seq in
        List.iter splitter act_idx;
        line_consumer (i+1)
    | exception End_of_file ->
        close_in in_chan;
    in
  line_consumer 1; 
  Printf.printf "%d\n" (Array.fold_left (fun acc x -> acc+x) 0 split_counter)
