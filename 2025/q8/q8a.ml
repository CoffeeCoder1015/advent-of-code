let to_coord_array raw = 
  let str_array_to_3d str_list = List.map (fun x -> int_of_string x) str_list in
  List.map (fun x -> String.split_on_char ',' x |> str_array_to_3d) raw
;;

let in_chan = open_in "test.txt" in
  let length = in_channel_length in_chan in
  let raw_coords = String.split_on_char '\n' ( really_input_string in_chan length ) in
  let coords = to_coord_array raw_coords in
  let sum acc x = acc+.x in
  let dist a b = List.fold_left sum 0. ( List.map2 (fun x y -> ( float_of_int (x-y) )**2.) a b) in
  let rec calc_dist acc remaining = 
    if List.is_empty remaining then
      acc
    else
    let focus = List.hd remaining in
    let rest = List.tl remaining in
    let dl = List.map (fun x -> (focus, x, dist focus x )) rest in
    let new_acc = acc @ dl in
    calc_dist new_acc rest
  in
  let dist_pairings = calc_dist [] coords in
  let pair_compare (_,_,dist1) (_,_,dist2) = int_of_float ( dist1-.dist2 ) in
  let print_func (a,b,dist) = Printf.printf "%d,%d,%d %d,%d,%d %f\n" (List.nth a 0) (List.nth a 1) (List.nth a 2) (List.nth b 0) (List.nth b 1) (List.nth b 2) dist in
  let sorted_pairings = List.sort pair_compare dist_pairings in
  List.iter print_func sorted_pairings;

