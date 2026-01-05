let in_chan = open_in "q11.txt" in
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
  let empty = Hashtbl.to_seq_keys graph |> Seq.map (fun a -> (a,0)) |> Hashtbl.of_seq in
  
  let init = 
    let tmp = Hashtbl.copy empty in
    Hashtbl.replace tmp "you" 1;
    tmp
  in

  let rec solve state = 
    let new_state = Hashtbl.copy empty in
    Hashtbl.iter (fun k v ->
      if v > 0 then
        let next_nodes = Hashtbl.find graph k in
        List.iter (fun node -> 
          let current = Hashtbl.find new_state node in 
          Hashtbl.replace new_state node (current + v)
          ) next_nodes;
      ) state; 
      
    let finished = Hashtbl.fold (fun k v acc -> if v = 0 || (v > 0 && k = "out") then acc else false ) new_state true in
    if finished then
      new_state
    else
      solve new_state
  in
  let res = solve init in
  Printf.printf "%d\n" (Hashtbl.find res "out")

