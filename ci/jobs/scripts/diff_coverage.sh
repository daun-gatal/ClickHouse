

CURRENT_COMMIT=$(git rev-parse HEAD)
BASE_COMMIT=$(git merge-base HEAD master)

# Try to find .info file from S3, checking up to 10 ancestor commits
FOUND=0
ATTEMPT=0
MAX_ATTEMPTS=10

while [ $ATTEMPT -lt $MAX_ATTEMPTS ]; do
    if [ $ATTEMPT -eq 0 ]; then
        TEST_COMMIT="${BASE_COMMIT}"
    else
        TEST_COMMIT=$(git rev-parse "${BASE_COMMIT}~${ATTEMPT}" 2>/dev/null || echo "")
        if [ -z "${TEST_COMMIT}" ]; then
            echo "Cannot get ancestor commit at depth ${ATTEMPT}"
            break
        fi
    fi
    
    COVERAGE_URL="https://clickhouse-builds.s3.amazonaws.com/REFs/master/7${TEST_COMMIT}/llvm_coverage_merge/llvm_coverage.info"
    echo "Checking coverage file for commit ${TEST_COMMIT} (attempt $((ATTEMPT + 1))/${MAX_ATTEMPTS})..."
    
    if wget --spider "${COVERAGE_URL}" 2>&1 | grep -q '200 OK'; then
        echo "Found coverage file at ${COVERAGE_URL}"
        wget "${COVERAGE_URL}" -O base_llvm_coverage.info
        BASE_COMMIT="${TEST_COMMIT}"
        FOUND=1
        break
    fi
    
    ATTEMPT=$((ATTEMPT + 1))
done

if [ $FOUND -eq 0 ]; then
    echo "Warning: Could not find coverage file after checking ${ATTEMPT} commits"
    echo "Skipping differential coverage analysis"
    exit 0
fi

# get diff between current commit and base commit
git diff ${BASE_COMMIT}..${CURRENT_COMMIT} --unified=3 > changes.diff

genhtml current_llvm_coverage.info \
  --baseline-file base_llvm_coverage.info \
  --diff-file changes.diff \
  --output-directory diff-html \
  --legend