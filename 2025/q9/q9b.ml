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
  (* let print_array arr = Array.iter (Printf.printf "%d ") arr; print_newline(); in
  print_array xa; print_array ya; *)
  let compressed_n = Array.length xa in
  
  let x_to_comped = Hashtbl.create compressed_n in
  let y_to_comped = Hashtbl.create compressed_n in
  
  let rec add_to_maps i =
    if i = compressed_n then
      ()
    else
      let current_x = xa.(i) in
      let current_y = ya.(i) in
      Hashtbl.add x_to_comped current_x i;
      Hashtbl.add y_to_comped current_y i;
    add_to_maps (i+1) 
    in
  add_to_maps 0;
  (x_to_comped,y_to_comped)
;;


let in_chan = open_in "test.txt" in
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
  (* Array.iter (fun ((a,b),(c,d),e) -> Printf.printf "(%d,%d) (%d,%d) %d\n" a b c d e ) acc; *)
  let xmap, ymap = get_compression_map coords in
  let x_range,y_range = Hashtbl.length xmap, Hashtbl.length ymap in
  let compress_coords (og_x,og_y) = 
    let new_x = Hashtbl.find xmap og_x in 
    let new_y = Hashtbl.find ymap og_y in
    (new_x,new_y)
  in
  let raster = Array.make_matrix x_range y_range 0 in
  let comprssed_coords = Array.map compress_coords coords in
  let loop_edges y = 
    (* find horizontal intervals and vertical intersections on y-scanline*)
    let rec helper i lst_v lst_h = 
      if i = n then
        lst_v, lst_h
      else
        let (curx,cury) = comprssed_coords.(i) in
        let (nxtx,nxty) = comprssed_coords.(( i+1 ) mod n) in
        let l,u = ( min cury nxty ), ( max cury nxty ) in
        if l <= y && y < u then
          helper (i+1) (curx :: lst_v) lst_h
        else if l = y && u =  y then
          helper (i+1) lst_v ((min curx nxtx,max curx nxtx) :: lst_h )
        else
          helper (i+1) lst_v lst_h
    in
    let vert_cords, hori_intervals =  ( fun ( x,y ) -> (Array.of_list x, Array.of_list y) )  ( helper 0 [] [] ) in
    Array.sort compare vert_cords;
    let vert_n = Array.length vert_cords in
    let rec form_intervals i intervals = 
      if i+1 >= vert_n then
        intervals
      else
        let current = vert_cords.(i) in
        let next = vert_cords.(i+1) in
        let new_arr = Array.append intervals [|(current,next)|]  in
        form_intervals (i+2) new_arr
    in
    (* go 2 by 2 to form the x intervals from the vertical intersections *)
    let vert_intervals = form_intervals 0 [||] in
    let all_intervals = Array.append vert_intervals hori_intervals in
    Array.sort (fun (a,_) (b,_) -> compare a b) all_intervals;
    let int_n = Array.length all_intervals in 
    let rec merge_intervals i acc = 
      if i = int_n then 
        acc
      else
        let (curL, curU) = all_intervals.(i) in
        if (i+1) = int_n then
          merge_intervals (i+1) (Array.append acc [|(curL, curU)|])
        else
        ( let (nL,nU) = all_intervals.(i+1) in
        if nL <= curU then
          ( all_intervals.(i+1) <- (curL,max curU nU);
          merge_intervals (i+1) acc; )
        else
          merge_intervals (i+1) (Array.append acc [|(curL, curU)|]); )
    in
    let merged_intervals = merge_intervals 0 [||] in
    merged_intervals
  in
  Array.iteri (fun i array -> 
    let x_intervals = loop_edges i in
    Array.iter (fun (s,e) -> 
      let delta = e-s+1 in 
      Array.fill array s delta 1;
       ) x_intervals
  ) raster;
  Array.iter (fun array -> 
    Array.iter (fun ele -> 
      match ele with
      | 0 -> Printf.printf("_")
      | 1 -> Printf.printf("O")
     ) array;
     print_newline()
  ) raster
  
