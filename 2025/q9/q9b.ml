module IntSet = Set.Make(Int)

let get_compression_map coord_arr =
  let n = Array.length coord_arr in
  let rec add_to_set i xs ys =
    if i = n then
      (xs,ys)
    else
      let (x,y) = coord_arr.(i) in
      let new_xs = IntSet.add x xs in
      let new_ys = IntSet.add y ys in
      add_to_set (i+1) new_xs new_ys
    in
  let xs,ys = add_to_set 0 IntSet.empty IntSet.empty in
  let intset_to_array set = set |> IntSet.to_list |> Array.of_list in
  let xa, ya = intset_to_array xs, intset_to_array ys in
  Array.sort compare xa; Array.sort compare ya;
  
  (* print arrays *)
  let print_array arr = Array.iter (Printf.printf "%d ") arr; print_newline(); in
  print_array xa; print_array ya;
;;


let in_chan = open_in "q9.txt" in
  let length = in_channel_length in_chan in
  let file = really_input_string in_chan length in
  let str_lst_to_xy str_lst = (List.nth str_lst 0 |> int_of_string, List.nth str_lst 1 |> int_of_string) in
  let coords = String.split_on_char '\n' file |> List.map (fun x -> String.split_on_char ',' x |> str_lst_to_xy) |> Array.of_list in
  let n = Array.length coords in
  let coord_area (a,b) (c,d) = ( abs ( a-c )  + 1 ) * (abs ( b-d ) + 1) in
  let rec par i acc = 
    if i = n then
        acc
    else
    let current = coords.(i) in
    let rec chi j acc2 = 
      let idx = j+i+1 in
      if idx = n then
          acc2
      else
      let focus = coords.(idx) in 
      let area = coord_area focus current in 
      chi (j+1) ( (current, focus, area) :: acc2 )
      in
      let acc2_result = chi 0 [] in
      let new_acc =Array.append (acc2_result |> Array.of_list) acc in
      par (i+1) new_acc
    in
    let acc = par 0 [||] in
    Array.sort (fun (_,_,a) (_,_,b) -> b-a) acc;
    Array.iter (fun ((a,b),(c,d),e) -> Printf.printf "(%d,%d) (%d,%d) %d\n" a b c d e ) acc;
    get_compression_map coords
    
