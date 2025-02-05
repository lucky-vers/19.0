import subprocess
from concurrent.futures import ProcessPoolExecutor, as_completed

def run_process(i):
    """Spawn a new process for each byte pair and check output."""
    process = subprocess.Popen(
        "./babyrev-level-19-0",
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL,  # Ignore errors
        text=True  # Decode stdout as text
    )

    process.stdin.write(str(int.to_bytes(i)))  # Send 2 bytes to stdin
    process.stdin.close()  # Close stdin to signal EOF
    output = process.stdout.read().strip()  # Read stdout
    process.wait()  # Wait for process to finish

    # Check if output does not contain "INCORRECT!"
    if "INCORRECT!" not in output:
        print(i, output)  # Return successful (i, j) and output
        exit(0)

    return None  # Indicate failure

# Limit parallel execution to 10 workers
with ProcessPoolExecutor(max_workers=10) as executor:
    future_to_input = {executor.submit(run_process, i): i for i in range(255)}

    for future in as_completed(future_to_input):
        result = future.result()
        if result:
            i, output = result
            print(f"SUCCESS: i={i}, output={output}")
            executor.shutdown(wait=False)  # Stop further execution
            break
