#![allow(unused_variables)]
#![allow(unused_must_use)]
use std::{cmp};
use std::collections::{HashMap};
use std::thread;
use std::sync::{RwLock, Arc, Mutex, Condvar};

const SEARCH_SIZE: u16 = 100;
static T: u16 = 1;
const THREAD_NUM: u16 = 64;
const THREAD_MASK: u16 = THREAD_NUM - 1;

fn main() {
    let mut solution = NetworkControl::new();
    solution.start();
    println!("t:{} search size:{}", T, SEARCH_SIZE);
    for sz in 1..=SEARCH_SIZE {
        *solution.current_size.write().unwrap() = sz;
        build_config_queues(sz, &mut solution.config_queues);
        notify_all(&solution.distri_sems);
        wait_all(&solution.gather_sems);
        notify_all(&solution.distri_sems);
        wait_all(&solution.gather_sems);
    }
    println!("zero graphs:");
    let zero_graph = solution.zero_graph.read().unwrap().clone();
    for (i, graph) in zero_graph.iter().enumerate() {
        println!("{}th zero_graph: {}", i, format_config(graph));
    }

    println!("Start verification");
    notify_all(&solution.distri_sems);
    wait_all(&solution.gather_sems);
    println!("Finish verification");
}

struct NetworkControl {
    f: Arc<Vec<Arc<RwLock<HashMap<String, u16> > > > >,
    zero_graph: Arc<RwLock<Vec<Vec<u16> > > >, 
    config_queues: Vec<Arc<RwLock<Vec<Vec<u16> > > > >,
    distri_sems: Vec<Arc<(Mutex<bool>, Condvar)> >,
    gather_sems: Vec<Arc<(Mutex<bool>, Condvar)> >,
    current_size: Arc<RwLock<u16> >
}

impl NetworkControl {
    fn new() -> Self{
        let mut _f = vec![];
        for i in 0..THREAD_NUM {
            let hm: HashMap<String, u16> = HashMap::new(); 
            _f.push(Arc::new(RwLock::new(hm.clone())));
        }
        let mut t = NetworkControl{
            f: Arc::new(_f.clone()), 
            zero_graph: Arc::new(RwLock::new(vec![vec![]])),
            config_queues: vec![],
            distri_sems: vec![],
            gather_sems: vec![],
            current_size: Arc::new(RwLock::new(0u16))
        };
        for i in 0..THREAD_NUM {
            t.distri_sems.push(Arc::new((Mutex::new(false), Condvar::new())));
            t.gather_sems.push(Arc::new((Mutex::new(false), Condvar::new())));
            t.config_queues.push(Arc::new(RwLock::new(vec![vec![]])));
        }
        t
    }  
    
    fn start(&mut self) {
        for thread_id in 0..THREAD_NUM {
            let thread_name = format!("{}", thread_id);
            let distri_sem = Arc::clone(&self.distri_sems[thread_id as usize]);
            let gather_sem = Arc::clone(&self.gather_sems[thread_id as usize]);
            let config_queue = Arc::clone(&self.config_queues[thread_id as usize]);
            let zero_graph = Arc::clone(&self.zero_graph);
            let f = Arc::clone(&self.f);
            let current_size = Arc::clone(&self.current_size);
            thread::Builder::new().name(thread_name).spawn(move|| {
                let thread_id = thread::current().name().unwrap().parse::<u16>().unwrap();
                loop {
                    let mut config_value: Vec< (Vec<u16>, u16)> = vec![];
                    wait(&distri_sem);
                    search_value(&config_queue, &mut config_value, &f, thread_id);
                    notify(&gather_sem);
                    wait(&distri_sem);
                    put_value(&config_value, &f, &zero_graph, thread_id);
                    notify(&gather_sem);
                    if current_size.read().unwrap().clone() == SEARCH_SIZE {
                        break;
                    }
                }
                wait(&distri_sem);
                let _f = f[thread_id as usize].read().unwrap().clone();
                for (paths, value) in _f {
                    if T == 1 {
                        if NetworkControl::compute(paths.encode_utf16().collect(), T) != value {
                            println!("counter case:{}; True value:{}; My value:{}", format_config(&paths.encode_utf16().collect()), value, NetworkControl::compute(paths.encode_utf16().collect(), T));
                        }
                    } else {
                        if (NetworkControl::compute(paths.encode_utf16().collect(), T) > 0) != (value > 0) {
                            println!("counter case:{}; True value:{}; My value:{}", format_config(&paths.encode_utf16().collect()), value > 0, NetworkControl::compute(paths.encode_utf16().collect(), T) > 0);
                        }
                    }
                }
                notify(&gather_sem);
            });
        }
    }

    fn isplayer0win(paths:Vec<u16>, t: u16) ->bool {
        let mut paths = paths;
        if t == 1{
            gfull_t1(&mut paths);
            return paths.is_empty() == false;
        }
        gsym(&mut paths);
        if paths == vec![1, 2*t+1, 2*t+2]  {
            return false;
        }
        true
    }

    fn is_ddc_t1(paths:Vec<u16>) -> bool {
        let mut paths = paths;
        gfull_t1(&mut paths);
        if paths.len() == 1 {
            return (paths[0] & 1) > 0 && paths[0] >= 3;
        }
        if paths.len() == 2 {
            return paths == vec![4u16, 7u16] || paths == vec![3u16,8u16] || paths == vec![1u16,10u16] || paths == vec![8u16,11u16] || paths == vec![5u16,14u16] || paths == vec![3u16,16u16] || paths[0] + 3 == paths[1];
        }
        if paths.len() == 3 {
            return paths == vec![1u16, 4u16, 6u16] || paths == vec![1u16,3u16,7u16] || paths == vec![5u16, 6u16, 8u16] || paths == vec![3u16, 5u16, 11u16] || paths == vec![4u16, 8u16, 9u16] || paths == vec![4u16, 5u16, 12u16] || paths == vec![1u16, 8u16, 12u16] || paths == vec![1u16, 5u16, 15u16] || paths == vec![1u16, 4u16, 16u16] || 3 + paths[0] + paths[1] == paths[2];
        }
        if paths.len() == 4 {
            if paths == vec![1u16, 5u16, 7u16, 8u16] || paths == vec![1u16, 4u16, 5u16, 11u16] {
                return true;
            }
            for i in 0..4 {
                for j in i+1..4 {
                    let mut w: Vec<u16> = vec![];
                    for k in 0..4 {
                        if k != i && k != j {
                            w.push(paths[k as usize]);
                        }
                    }
                    let i = i as usize;
                    let j = j as usize;
                    if w == vec![3u16, 4u16] && paths[i] + 4 == paths[j] {
                        return true;
                    }
                    if w == vec![3u16, 5u16] && paths[i] + 11 == paths[j] {
                        return true;
                    }
                    if w == vec![1u16, 4u16] && paths[i] + 6 == paths[j] {
                        return true;
                    }
                    if w == vec![1u16, 3u16] && paths[i] + 7 == paths[j] {
                        return true;
                    }
                    if w == vec![5u16, 8u16] && paths[i] + 6 == paths[j] {
                        return true;
                    }
                    if w == vec![3u16, 8u16] && paths[i] + 8 == paths[j] {
                        return true;
                    }
                }
            }
            return false;
        }
        if paths.len() == 5 {
            for i in 0..5 {
                for j in i+1..5 {
                    let mut w: Vec<u16> = vec![];
                    for k in 0..5 { 
                        if k != i && k != j {
                            w.push(paths[k as usize]);
                        }
                    }
                    let i = i as usize;
                    let j = j as usize;
                    if w == vec![4u16, 5u16, 8u16] && paths[i] + 4 == paths[j] {
                        return true;
                    }
                    if w == vec![1u16, 5u16, 8u16] && paths[i] + 7 == paths[j] {
                        return true;
                    }
                    if w == vec![1u16, 4u16, 8u16] && paths[i] + 8 == paths[j] {
                        return true;
                    }
                    if w == vec![1u16, 4u16, 5u16] && paths[i] + 11 == paths[j] {
                        return true;
                    }
                }
            }
            return false;
        }
        false
    }
    
    fn compute(paths:Vec<u16>, t:u16) -> u16 {
        let mut paths = paths;
        if t > 1 {
            return NetworkControl::isplayer0win(paths.clone(), t) as u16;
        }
        gfull_t1(&mut paths);
        let mut parity = 0u16;
        for i in &paths {
            parity ^= *i;
        } 
        if parity & 1 == 0 {
            if NetworkControl::isplayer0win(paths.clone(), t) {
                return 2u16;
            }
            return 0u16;
        }
        if NetworkControl::is_ddc_t1(paths.clone()) {
            return 3u16;
        }
        return 1u16;
    }

}

fn config2threadid(d:& Vec<u16>) -> u16 {
    let mut id = 0u16;
    for i in d {
        id=(id<<3)^(id>>4)^*i; 
    }
    id & THREAD_MASK
}

fn gsym(d:&mut Vec<u16>) {
    if d.is_empty() {
        return;
    }
    d.sort();
    let mut l:i32 = 0;
    for i in 1..d.len() {
        if l == -1 || d[i] != d[l as usize] {
            l += 1;
            d[l as usize] = d[i];
        } else {
            l -= 1;
        }
    }
    d.resize((l + 1) as usize, 0u16);
}

fn gfull_t1(d:&mut Vec<u16>) {
    gsym(d);
    let mut w = vec![-1; 10];
    for (index, i) in d.iter().enumerate() {
        if *i > 8 {
            break;
        }
        if *i == 1 || *i == 3 || *i == 4 || *i == 5 || *i == 8{
            w[*i as usize] = index as i8;
        }
    }
    if w[1] != -1 && w[3] != -1 && w[4] != -1 {
        d[w[1] as usize] = 0;
        d[w[3] as usize] = 0;
        d[w[4] as usize] = 0;
    } else {
        if w[3] != -1 && w[5] != -1 && w[8] != -1 {
            d[w[3] as usize] = 0;
            d[w[5] as usize] = 0;
            d[w[8] as usize] = 0;
        } else {
            if w[1] != -1 && w[4] != -1 && w[5] != -1 && w[8] != -1 {
                d[w[1] as usize] = 0;
                d[w[4] as usize] = 0;
                d[w[5] as usize] = 0;
                d[w[8] as usize] = 0;
            }
        }
    }
    let mut tmp = vec![];
    for i in &mut *d {
        if *i > 0 {
            tmp.push(*i);
        }
    }  
    *d = tmp.clone();
}


fn notify(sem:& (Mutex<bool>, Condvar)) {
    let (lock, cvar) = &*sem;    
    *lock.lock().unwrap() = true;
    cvar.notify_one();
}

fn notify_all(sems:& Vec<Arc<(Mutex<bool>, Condvar)> >) {
    for sem in sems {
        notify(sem);
    }
}

fn wait(sem:& (Mutex<bool>, Condvar)) {
    let (lock, cvar) = &*sem;
    let mut started = *lock.lock().unwrap();
    while !started {
        started = *cvar.wait(lock.lock().unwrap()).unwrap();
    }
    *lock.lock().unwrap() = false;
}

fn wait_all(sems:& Vec<Arc<(Mutex<bool>, Condvar)> >) {
    for sem in sems {
        wait(sem);
    }
}


fn split(n: u16,mn: u16,d: &mut Vec<u16>, config_queues:&mut Vec<Vec<Vec<u16> > >) {
    if n==0 {
        config_queues[config2threadid(d) as usize].push(d.clone());
        return;
    }
    if mn>n {
        return;
    }
    for i in mn..=n/2 {
        d.push(i);
        split(n-i, i+1, d, config_queues);
        d.pop();
    }
    d.push(n);
    split(0, n+1, d, config_queues);
    d.pop();
}

fn build_config_queues(sz: u16, config_queues:&mut Vec<Arc<RwLock<Vec<Vec<u16> > > > >) {
    let mut d: Vec<u16> = vec![];
    let mut config_queues_:Vec<Vec<Vec<u16>>> = vec![vec![]; THREAD_NUM as usize];
    split(sz, 1, &mut d, &mut config_queues_);
    for i in 0..THREAD_NUM {
        *config_queues[i as usize].write().unwrap() = config_queues_[i as usize].clone();
    }
}

fn search_value(config_queue:& Arc< RwLock< Vec<Vec<u16> > > >,config_value:&mut Vec< (Vec<u16>, u16)>, f:&Vec<Arc<RwLock<HashMap<String, u16> > > >, thread_id: u16) {
    let config_queue = config_queue.read().unwrap().clone();
    for config in &config_queue {
        let mut d = vec![];
        let mut v = 0;
        'outfor:for (i, di) in config.iter().enumerate() {
            for j in 1..=(*di+1)/2 {
                let mut u = d.clone();
                let mut vv = 1;
                if j > T+1 {
                    u.push(j-T-1);
                    vv += T;
                } else {
                    vv += j-1;
                }
                if j + T < *di {
                    u.push(*di-j-T);
                    vv += T;
                } else {
                    vv += *di-j;
                }
                if vv <= v {
                    continue;
                }
                for k in i+1..config.len() {
                    u.push(config[k]);
                }
                gsym(&mut u);
                if u.is_empty() {
                    v = vv;
                } else {
                    let key: String = (String::from_utf16_lossy(&u).to_string()).clone();
                    let oponent_v = *((f[config2threadid(&u) as usize]).read().unwrap().get(&key)).unwrap();
                    if oponent_v < vv {
                        v = cmp::max(v, vv-oponent_v);
                    }
                }
                if v == T*2+1 {
                    break 'outfor;
                }
            }
            d.push(*di);
        }
        config_value.push((config.clone(), v));
    } 
}

fn put_value(config_value:& Vec< (Vec<u16>, u16)>, f:&Vec<Arc<RwLock<HashMap<String, u16> > > >, zero_graph:& Arc<RwLock<Vec<Vec<u16> > > >, thread_id: u16) {
    let mut _f = f[thread_id as usize].write().unwrap();
    for (config, value) in config_value {
        _f.insert((String::from_utf16_lossy(&config).to_string()).clone(), *value);
        if *value == 0 {
            zero_graph.write().unwrap().push(config.clone());
        }
    }
}

fn format_config(config:& Vec<u16>) -> String {
    let mut str: String = "".to_string();
    for i in config {
        str = format!("{} {}", str, (*i as u32).to_string());
    }
    str
}


