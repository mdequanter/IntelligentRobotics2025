import sys, time, math, argparse
from spherov2 import scanner
from spherov2.sphero_edu import SpheroEduAPI
from spherov2.types import Color
from spherov2.commands.power import Power

SEGMENTS_CM = [200, 200, 100, 100, 150, 100, 100, 200, 250]     # afstand
HEADINGS    = [  0,  90, 180, 270, 180,  90, 180, 270,   0]    # richting (graden)

# SPEED + CM/S
SPEED_PCT_DEFAULT = 70                     
CM_PER_SEC_DEFAULT = 41.7                   

# BRAKE
RAMP = 0.35
BRAKE = 0.20

# LED
LED_READY = Color(0, 0, 255)
LED_RUN   = Color(255, 120, 0)
LED_OK    = Color(0, 255, 0)
LED_ERR   = Color(255, 0, 0)


def find_toy(name_or_mac: str):
    toys = scanner.find_toys()
    for t in toys:
        if t.name == name_or_mac or getattr(t, "address", "") == name_or_mac:
            return t
    return None

def seconds_for_distance(dist_cm: float, cmps: float) -> float:
    return max(0.0, dist_cm / max(cmps, 1.0))

def print_battery(api, toy_name: str):
    try:
        v = Power.get_battery_voltage(api.toy)
        print(f"[{toy_name}] Battery: {v:.2f} V")
        if v <= 3.5:
            print("Batterie LOW")
    except Exception:
        pass

def gentle_roll(api: SpheroEduAPI, heading: int, speed_pct: int, seconds: float):
    
    if seconds <= 0:
        return
    # ramp-up
    api.roll(heading, speed_pct, RAMP)
    # cruise
    cruise = max(0.0, seconds - 2*RAMP)
    if cruise > 0:
        api.roll(heading, speed_pct, cruise)
    # ramp-down + petit frein
    api.roll(heading, max(10, int(0.4 * speed_pct)), RAMP)
    api.roll(heading, 0, BRAKE)

def calibrate_zero(api: SpheroEduAPI):
    print("Calibration: 0° then ENTER")
    try:
        api.start_calibration()
    except Exception:
        pass
    input()
    try:
        api.finish_calibration()
    except Exception:
        api.set_heading(0)
    print("0°\n")

def run_lap(api: SpheroEduAPI, segments_cm, headings, cmps: float, speed_pct: int):
    api.set_stabilization(True)
    api.set_back_led(255)
    api.set_main_led(LED_RUN)

    # départ
    for k in (3,2,1):
        print(f"… {k}")
        api.set_main_led(Color(255,255,0)); time.sleep(0.3)
        api.set_main_led(Color(0,0,0));     time.sleep(0.4)
    print("GO!")
    api.set_main_led(LED_RUN)

    t0 = time.perf_counter()

    for hdg, dist in zip(headings, segments_cm):
        api.set_heading(hdg)   # virage instantané pour rester serré
        secs = seconds_for_distance(dist, cmps)
        print(f"→ {dist:.0f} cm @ {hdg:3d}°  (~{secs:.2f}s)")
        gentle_roll(api, hdg, speed_pct, secs)

    api.roll(0,0,0.1)
    t1 = time.perf_counter()
    lap = t1 - t0
    api.set_back_led(0)
    api.set_main_led(LED_OK)
    print(f"\n⏱️  Lap time: {lap:.3f} s")
    return lap


def main():
    p = argparse.ArgumentParser(description="Sphero BOLT — Autonome ronde (horaire)")
    p.add_argument("--name", required=True, help="Nom du SPHERO (ex: SB-9DD8)")
    p.add_argument("--speed", type=int, default=SPEED_PCT_DEFAULT, help=" SPEED % (0–100)")
    p.add_argument("--cmps", type=float, default=CM_PER_SEC_DEFAULT, help="cm/s")
    p.add_argument("--segments", type=str, default=",".join(str(x) for x in SEGMENTS_CM),
                   help="Segments in cm")
    args = p.parse_args()

    segments = [float(x) for x in args.segments.split(",") if x.strip()]
    toy = find_toy(args.name)
    if toy is None:
        print(f"NO SPHERO '{args.name}'")
        sys.exit(1)

    print(f"Connected {toy.name}")
    try:
        with SpheroEduAPI(toy) as api:
            api.set_main_led(LED_READY)
            print_battery(api, toy.name)
            calibrate_zero(api)
            _ = run_lap(api, segments, HEADINGS, args.cmps, max(10, min(100, args.speed)))
            api.set_main_led(LED_OK)
    except KeyboardInterrupt:
        try:
            with SpheroEduAPI(toy) as api:
                api.roll(0,0,0.2); api.set_main_led(LED_ERR)
        except Exception:
            pass
        print("\n STOPPED")
    except Exception as e:
        print(f" Error {e}")
        try:
            with SpheroEduAPI(toy) as api:
                api.roll(0,0,0.2); api.set_main_led(LED_ERR)
        except Exception:
            pass
        sys.exit(2)

if __name__ == "__main__":
    main()
