let in_chan = open_in "test.txt" in
  let graph = Hashtbl.create 10 in
  let rec line_consumer _ = 
    match input_line in_chan with
    | line -> 

      let n = String.length line in
      let node = String.sub line 0 3 in
      let leaves = String.sub line 4 (n-4) |> String.trim |> String.split_on_char ' ' in
      Hashtbl.add graph node leaves;
      line_consumer();
    | exception End_of_file -> 
        ()
  in
  line_consumer();
  
  Hashtbl.add graph "out" ["out"];
  let empty = Hashtbl.to_seq_keys graph |> Seq.map (fun a -> (a,[||])) |> Hashtbl.of_seq in
  
  let init = 
    let tmp = Hashtbl.copy empty in
    Hashtbl.replace tmp "svr" [|(false,false)|];
    tmp
  in

  let dac_transform (_,b) = (true,b) in
  let fft_transform (a,_) = (a,true) in
  let rec solve state = 
    let new_state = Hashtbl.copy empty in
    Hashtbl.iter (fun k v ->
      if Array.length v > 0 then
        let next_nodes = Hashtbl.find graph k in
        List.iter (fun node -> 
          let current = Hashtbl.find new_state node in 
          match node with
          | "dac" -> 
            let new_v = Array.map dac_transform v in
            Hashtbl.replace new_state node (Array.append current new_v)
          | "fft" ->
            let new_v = Array.map fft_transform v in
            Hashtbl.replace new_state node (Array.append current new_v)
          | _ ->
            Hashtbl.replace new_state node (Array.append current v)
          ) next_nodes;
      ) state; 
      
    let finished = Hashtbl.fold (fun k v acc -> if Array.length v = 0 || (Array.length v > 0 && k = "out") then acc else false ) new_state true in
    if finished then
      new_state
    else
      solve new_state
  in
  let res = solve init in
  let out_chan = Hashtbl.find res "out" in
  let reached_dac_fft = Array.to_list out_chan |> List.filter (fun (a,b) -> a && b) in
  Printf.printf "%d\n" (List.length reached_dac_fft)

