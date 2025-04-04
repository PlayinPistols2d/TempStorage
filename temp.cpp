CREATE OR REPLACE FUNCTION upsert_pm(
    in_nm TEXT,
    in_desc TEXT,
    in_fltr JSONB,
    in_dev_id INTEGER,
    in_type INTEGER,
    in_io INTEGER
) RETURNS INTEGER AS
$$
DECLARE
    existing_id INTEGER;
    existing_fltr JSONB;
    new_fltr JSONB;
    report_type INT;
    existing_var_part TEXT;
    new_var_part TEXT;
    last_number INT;
    parts TEXT[];
BEGIN
    SELECT id, fltr INTO existing_id, existing_fltr
    FROM public.pm
    WHERE nm = in_nm;

    IF existing_id IS NOT NULL THEN
        -- Check if report_type matches
        report_type := (in_fltr ->> 'report_type')::INT;

        IF (existing_fltr ->> 'report_type')::INT = report_type THEN
            existing_var_part := existing_fltr ->> 'var_part';

            -- Split and get next number
            parts := string_to_array(existing_var_part, '#');
            last_number := parts[array_length(parts, 1)]::INT + 1;
            new_var_part := existing_var_part || '#' || last_number::TEXT;

            new_fltr := jsonb_set(in_fltr, '{var_part}', to_jsonb(new_var_part::TEXT), false);
        ELSE
            new_fltr := in_fltr;
        END IF;

        UPDATE public.pm
        SET fltr = new_fltr,
            "desc" = in_desc,
            type = in_type,
            io = in_io,
            sv_dev = in_dev_id
        WHERE id = existing_id
        RETURNING id INTO existing_id;

        RETURN existing_id;
    ELSE
        INSERT INTO public.pm(nm, "desc", fltr, sv_dev, glb, type, io, en)
        VALUES (
            in_nm,
            in_desc,
            in_fltr,
            in_dev_id,
            FALSE,
            in_type,
            in_io,
            TRUE
        )
        RETURNING id INTO existing_id;

        RETURN existing_id;
    END IF;
END;
$$ LANGUAGE plpgsql;





SELECT upsert_pm(
    'SignalA',
    'Final call with correct types',
    '{
        "type": "name1",
        "numbit": 12,
        "offset": 6,
        "numword": 9,
        "countbit": 2,
        "var_part": "70",
        "report_type": 7,
        "internalname": "somename1"
    }'::jsonb,
    3,  -- sv_dev (device ID as INTEGER)
    1,  -- type
    0   -- io
);