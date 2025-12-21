let in_chan = open_in "test.txt" in
  let length = in_channel_length in_chan in
  let file = really_input_string in_chan length in
  let str_lst_to_xy str_lst = (List.nth str_lst 0 |> int_of_string, List.nth str_lst 1 |> int_of_string) in
  let coords = String.split_on_char '\n' file |> List.map (fun x -> String.split_on_char ',' x |> str_lst_to_xy) |> Array.of_list in
  let n = Array.length coords in
  let coord_area (a,b) (c,d) = ( abs ( a-c )  + 1 ) * (abs ( b-d ) + 1) in
  let get_other_diag (a,b) (c,d) = ((a,d),(c,b)) in
  let check_in (a,b) =
    let rec find_x i acc = 
      if i = n then
        acc
      else
      let next = (i+1) mod n in
      let ( cx,cur_y ) = coords.(i) in
      let ( nx,nxt_y ) = coords.(next) in
      let lower = min cur_y nxt_y in
      let upper = max cur_y nxt_y in
      if lower <= b && b <= upper then
       find_x (i+1) ( acc @ [cx] @ [nx] )
      else
       find_x (i+1) acc
    in
    let res = find_x 0 [] in
    let sres = List.sort compare res in
    let rec check_x lst = 
      let arrayed = Array.of_list lst in
      let arr_len = Array.length arrayed in
      let rec helper i =
        if i = arr_len-1 then 
          false
        else
          let current = arrayed.(i) in 
          let next = arrayed.(i+1) in
          if current <= a && a <= next then
            true 
          else
            helper (i+1)
          in
          helper 0 
        in
    let result = check_x sres in
    (* List.iter (Printf.printf "%d ") sres;
    print_newline ();
    Printf.printf "%B\n" result; *)
    result
  in 
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
      let area = coord_area focus current in 
      let (other_a,other_b) = get_other_diag focus current in
      let all_in = ( check_in other_a  ) &&  ( check_in other_b ) in
      ( fun (a,b) (c,d) (e,f) (g,h) -> Printf.printf "(%d,%d) (%d,%d) (%d,%d) (%d,%d)" a b c d e f g h ) current focus other_a other_b;
      Printf.printf " %d %B\n" area all_in;
      if ( area > best_c ) && all_in then
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
    