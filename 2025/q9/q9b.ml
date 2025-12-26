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
  Array.iteri (fun i x -> if i > 0 then raster.(0).(i) <- raster.(0).(i-1)+x) raster.(0); (* SAT vert *)
  Array.iteri (fun i x -> if i > 0 then raster.(i).(0) <- raster.(i-1).(0)+x.(0)) raster; (* SAT hori *)
  let rec build_sat x y =
    if y = y_range then
      ()
    else
      let left = raster.(x-1).(y) in
      let top = raster.(x).(y-1) in
      let diag = raster.(x-1).(y-1) in
      let current = raster.(x).(y) in
      raster.(x).(y) <- current + left + top - diag;
      if x+1 = x_range then
        build_sat 1 (y+1)
      else
        build_sat (x+1) y
  in
  build_sat 1 1;

  (* Array.iter (fun a -> Array.iter (fun x -> Printf.printf "%d " x) a; print_newline()) raster; *)
  let acc_n = Array.length acc in
  let rec filter_max i = 
    if i = acc_n then
      -1
    else
    let (ra,rb),(rc,rd),real_area = acc.(i) in
    let (a,b),(c,d) = compress_coords (ra,rb), compress_coords (rc, rd) in
    let test_area = coord_area (a,b) (c,d) in
    let verts = [|(a,b);(c,d);(a,d);(c,b)|] in 
    Array.sort ( fun (x1,y1) (x2,y2) -> (x1+y1)-(x2+y2) ) verts;
    verts.(0) <- ( verts.(0) |> fun (x,y) -> (x-1,y-1) );
    let (mx,my) = min verts.(1) verts.(2) in
    let (nx,ny) = max verts.(1) verts.(2) in
    verts.(1) <- (mx-1,my);
    verts.(2) <- (nx,ny-1);
    let areas = Array.map (fun (x,y) -> if x < 0 || y < 0 then 0 else raster.(y).(x)) verts in
    areas.(1) <- areas.(1) * -1;
    areas.(2) <- areas.(2) * -1;
    let sat_area = Array.fold_left (fun acc x -> acc+x ) 0 areas in
    if test_area = sat_area then
      real_area
    else
    filter_max (i+1)
  in
  let ans = filter_max 0 in
  Printf.printf "%d\n" ans;