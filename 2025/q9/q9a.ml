
let in_chan = open_in "q9.txt" in
  let length = in_channel_length in_chan in
  let file = really_input_string in_chan length in
  let str_lst_to_xy str_lst = (List.nth str_lst 0 |> int_of_string, List.nth str_lst 1 |> int_of_string) in
  let coords = String.split_on_char '\n' file |> List.map (fun x -> String.split_on_char ',' x |> str_lst_to_xy) |> Array.of_list in
  let n = Array.length coords in
  let coord_area (a,b) (c,d) = ( abs ( a-c )  + 1 ) * (abs ( b-d ) + 1) in
  let rec par i best = 
    if i = n then
      best
    else
    let current = coords.(i) in
    let rec chi j best_c = 
      let idx = j+i+1 in
      if idx = n then
        best_c
      else
      let focus = coords.(idx) in 
      (* ( fun (a,b) (c,d) -> Printf.printf "(%d,%d) (%d,%d)\n" a b c d ) current focus; *)
      let area = coord_area focus current in 
      if area > best_c then
        chi (j+1) area
      else
        chi (j+1) best_c
      in
    let best_sub_area = chi 0 best in
    if best_sub_area > best then
      par (i+1) best_sub_area
    else
      par (i+1) best
    in
    let ans = par 0 0 in
    Printf.printf "%d\n" ans;
    